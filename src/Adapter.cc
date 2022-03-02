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

thread_local pEp::Adapter::Session pEp::Adapter::session;

namespace pEp {
    void throw_status(::PEP_STATUS status)
    {
        if (status == ::PEP_STATUS_OK) {
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

    namespace Adapter {
        std::thread _sync_thread;
        ::utility::locked_queue<SYNC_EVENT, ::free_Sync_event> sync_evt_q;
        std::mutex mut;

        // private
        std::thread::id sync_thread_id()
        {
            return _sync_thread.get_id();
        }

        // private
        int _process_sync_event(::SYNC_EVENT ev, void *management)
        {
            if (ev != nullptr) {
                ::do_sync_protocol_step(session(), nullptr, ev);
                return 0;
            } else {
                return 0;
            }
        }

        // public (json adapter needs it, but should use Session mgmt from libpEpAdapter eventually)
        int _inject_sync_event(::SYNC_EVENT ev, void *management)
        {
            try {
                if (ev == nullptr) {
                    sync_evt_q.clear();
                    sync_evt_q.push_back(ev);
                } else {
                    sync_evt_q.push_front(ev);
                }
            } catch (exception &) {
                return 1;
            }
            return 0;
        }

        // private
        PEP_STATUS _ensure_passphrase(::PEP_SESSION session, const char *fpr)
        {
            return passphrase_cache.ensure_passphrase(session, fpr);
        }

        // public
        ::SYNC_EVENT _retrieve_next_sync_event(void *management, unsigned threshold)
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

        // ---------------------------------------------------------------------------------------
        Session::Session() :
            _messageToSend{ nullptr }, _notifyHandshake{ nullptr }, _sync_mode{ SyncModes::Async },
            _adapter_manages_sync_thread{ false }
        {
        }

        void Session::initialize(SyncModes sync_mode, bool adapter_manages_sync_thread)
        {
            pEpLog("Initializing session with CallbackDispatcher...");
            _init(
                pEp::CallbackDispatcher::messageToSend,
                pEp::CallbackDispatcher::notifyHandshake,
                sync_mode,
                adapter_manages_sync_thread);
        }

        void Session::initialize(
            SyncModes sync_mode,
            bool adapter_manages_sync_thread,
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake)
        {
            pEpLog("Initializing session...");
            _init(messageToSend, notifyHandshake, sync_mode, adapter_manages_sync_thread);
        }

        void Session::_init(
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake,
            SyncModes sync_mode,
            bool adapter_manages_sync_thread)
        {
            // cache the values for sync-thread session creation
            _messageToSend = messageToSend;
            _notifyHandshake = notifyHandshake;
            _sync_mode = sync_mode;
            _adapter_manages_sync_thread = adapter_manages_sync_thread;
            refresh();
            ::adapter_group_init();
        }

        void Session::refresh()
        {
            std::lock_guard<mutex> lock(mut);
            release();

            // Switch to mode "Sync" ensures the sync thread to be shutdown
            if (_sync_mode == SyncModes::Sync) {
                // process the event directly
                _inject_action = _process_sync_event;
                if (!_adapter_manages_sync_thread) {
                    stop_sync();
                } else {
                    // The adapter needs to shutdown sync thread
                }
            }
            // Switch to mode "ASync", sync thread needs to be started using start_sync
            if (_sync_mode == SyncModes::Async) {
                // put the event on queue
                _inject_action = _inject_sync_event;
            }

            // create
            ::PEP_SESSION session_;
            ::PEP_STATUS status;
            status = ::init(&session_, _messageToSend, _inject_action, _ensure_passphrase);
            throw_status(status);
            status = ::register_sync_callbacks(
                session_,
                nullptr,
                _notifyHandshake,
                _retrieve_next_sync_event);
            if (status != PEP_STATUS_OK) {
                pEpLog("libpEpAdapter: WARNING - session is initialized but without sync/callbacks. "
                       "This is normal if there are no own identities yet. Call session.init() again to "
                       "re-initialize the session after creating an own identity.");
            }
            // store
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
            } else {
                return _session.get();
            }
        }

        // public
        void inject_sync_shutdown() {
            pEpLog("called");
            _inject_sync_event(nullptr, nullptr);
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
                pEp::callback_dispatcher.notifyHandshake(nullptr, nullptr, SYNC_NOTIFY_STOP);
            }
        }

        // public
        bool is_sync_running()
        {
            if (!session._adapter_manages_sync_thread) {
                return _sync_thread.joinable();
            } else {
                return false;
            }
        }

        // public
        // Works even if adapter is managing sync thread, BUT must be using this queue
        bool in_shutdown()
        {
            SYNC_EVENT ev;
            try {
                ev = sync_evt_q.back();
            } catch (std::underflow_error &) {
                return false;
            }
            if (ev) {
                return false;
            } else {
                return true;
            }
        }
    } // namespace Adapter
} // namespace pEp
