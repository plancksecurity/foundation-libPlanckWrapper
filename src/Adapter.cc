// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "Adapter.hh"
#include <sstream>
#include "status_to_string.hh"
#include <pEp/pEpLog.hh>
#include "passphrase_cache.hh"
#include "callback_dispatcher.hh"
#include "group_manager_api.h"

using namespace std;

thread_local pEp::Adapter::Session pEp::Adapter::session{};

namespace pEp {
    namespace Adapter {
        ::SYNC_EVENT _cb_dequeue_next_sync_event(void *management, unsigned threshold);

        // ---------------------------------------------------------------------------------------
        // SESSION
        // ---------------------------------------------------------------------------------------
        std::mutex mut{};
        SyncModes Session::_sync_mode{ SyncModes::Async };
        bool Session::_adapter_manages_sync_thread{ false };
        ::messageToSend_t Session::_cb_messageToSend{ nullptr };
        ::notifyHandshake_t Session::_cb_notifyHandshake{ nullptr };
        ::inject_sync_event_t Session::_cb_handle_sync_event_from_engine{ nullptr };
        bool Session::_is_initialized{ false };

        Session::Session()
        {
            pEpLog("libpEpAdapter Session-manager created");
        }

        // Public/Static
        void Session::initialize(SyncModes sync_mode, bool adapter_manages_sync_thread)
        {
            pEpLog("Initializing session with CallbackDispatcher...");
            _init(
                pEp::CallbackDispatcher::messageToSend,
                pEp::CallbackDispatcher::notifyHandshake,
                sync_mode,
                adapter_manages_sync_thread);
        }

        // Public/Static
        void Session::initialize(
            SyncModes sync_mode,
            bool adapter_manages_sync_thread,
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake)
        {
            pEpLog("Initializing session...");
            _init(messageToSend, notifyHandshake, sync_mode, adapter_manages_sync_thread);
        }

        // Private/Static
        void Session::_init(
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake,
            SyncModes sync_mode,
            bool adapter_manages_sync_thread)
        {
            // cache the values for sync-thread session creation
            _cb_messageToSend = messageToSend;
            _cb_notifyHandshake = notifyHandshake;
            _sync_mode = sync_mode;
            _adapter_manages_sync_thread = adapter_manages_sync_thread;
            ::adapter_group_init();
            _is_initialized = true;
        }

        void Session::_new()
        {
            std::lock_guard<mutex> lock(mut);

            if (_sync_mode == SyncModes::Sync) {
                _cb_handle_sync_event_from_engine = _cb_pass_sync_event_to_engine;
            }

            if (_sync_mode == SyncModes::Async) {
                _cb_handle_sync_event_from_engine = _cb_enqueue_sync_event;
            }

            // create
            ::PEP_SESSION session_{ nullptr };
            ::PEP_STATUS status = ::init(&session_, _cb_messageToSend,
                _cb_handle_sync_event_from_engine, _ensure_passphrase);
            throw_status(status);

            status = ::register_sync_callbacks(
                session_,
                nullptr,
                _cb_notifyHandshake,
                _cb_dequeue_next_sync_event);
            throw_status(status);

            _session = SessionPtr{ session_, ::release };
        }

        void Session::release()
        {
            if (_session.get()) {
                _session = nullptr;
            }
        }

        // public
        ::PEP_SESSION Session::operator()()
        {
            if (!_session.get()) {
                throw std::runtime_error(
                    "libpEpAdapter: No session! Before use, call session.initialize() for each thread");
            }
            return _session.get();
        }

        // ---------------------------------------------------------------------------------------
        // SYNC
        // ---------------------------------------------------------------------------------------
        std::thread _sync_thread{};
        ::utility::locked_queue<SYNC_EVENT, ::free_Sync_event> sync_evt_q{};

        // private
        std::thread::id sync_thread_id()
        {
            return _sync_thread.get_id();
        }

        // private
        int _cb_pass_sync_event_to_engine(::SYNC_EVENT ev, void *management)
        {
            if (ev != nullptr) {
                ::do_sync_protocol_step(session(), nullptr, ev);
            }
            return 0;
        }

        // public (json adapter needs it, but should use Session mgmt from libpEpAdapter eventually)
        int _cb_enqueue_sync_event(::SYNC_EVENT ev, void *management)
        {
            try {
                if (ev == nullptr) {
                    sync_evt_q.clear();
                    sync_evt_q.push_back(ev);
                } else {
                    sync_evt_q.push_front(ev);
                }
            } catch (...) {
                return 1;
            }
            return 0;
        }

        // public
        ::SYNC_EVENT _cb_dequeue_next_sync_event(void *management, unsigned threshold)
        {
            ::SYNC_EVENT syncEvent = nullptr;
            const bool success = sync_evt_q.try_pop_front(syncEvent, std::chrono::seconds(threshold));

            if (!success) {
                return ::new_sync_timeout_event();
            }

            return syncEvent;
        }

        // public
        bool on_sync_thread()
        {
            return _sync_thread.get_id() == this_thread::get_id();
        }

        // public
        void inject_sync_shutdown()
        {
            pEpLog("called");
            _cb_enqueue_sync_event(nullptr, nullptr);
        }

        void start_sync()
        {
            startup<pEp::CallbackDispatcher>(
                &callback_dispatcher,
                &::pEp::CallbackDispatcher::on_startup,
                &::pEp::CallbackDispatcher::on_shutdown);
        }

        // public
        void stop_sync()
        {
            pEpLog("called");
            if (_sync_thread.joinable()) {
                pEpLog("sync_is_running - injecting null event");
                inject_sync_shutdown();
                _sync_thread.join();
                pEp::CallbackDispatcher::notifyHandshake(nullptr, nullptr, SYNC_NOTIFY_STOP);
            }
        }

        // public
        bool is_sync_running()
        {
            if (!session.adapter_manages_sync_thread()) {
                return _sync_thread.joinable();
            }
            return false;
        }

        // public
        bool in_shutdown()
        {
            SYNC_EVENT ev{ nullptr };
            try {
                ev = sync_evt_q.back();
            } catch (std::underflow_error &) {
                return false;
            }
            return !ev;
        }

        PEP_STATUS _ensure_passphrase(::PEP_SESSION session, const char *fpr)
        {
            return passphrase_cache.ensure_passphrase(session, fpr);
        }

    } // namespace Adapter

    void throw_status(::PEP_STATUS status)
    {
        if (status == ::PEP_STATUS_OK) {
            return;
        }
        if (status == ::PEP_KEY_IMPORTED) {
            return;
        }
        if (status >= 0x400 && status <= 0x4ff) {
            return;
        }
        if (status == ::PEP_STATEMACHINE_CANNOT_SEND) {
            return;
        }
        if (status == ::PEP_OUT_OF_MEMORY) {
            throw bad_alloc();
        }
        if (status == ::PEP_ILLEGAL_VALUE) {
            throw invalid_argument("illegal value");
        }

        string _status = status_to_string(status);
        throw RuntimeError(_status, status);
    }

    RuntimeError::RuntimeError(const std::string &_text, ::PEP_STATUS _status) :
        std::runtime_error(_text.c_str()), text(_text), status(_status)
    {
    }
} // namespace pEp
