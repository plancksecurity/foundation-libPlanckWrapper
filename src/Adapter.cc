// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "Adapter.hh"
#include <sstream>
#include <iomanip>
#include <assert.h>
#include "status_to_string.hh"
#include "pEpLog.hh"
#include "passphrase_cache.hh"

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

    RuntimeError::RuntimeError(const std::string &_text, ::PEP_STATUS _status)
        : std::runtime_error(_text.c_str()), text(_text), status(_status)
    {
    }

    namespace Adapter {
        // private
        SyncModes _sync_mode = SyncModes::Async;
        ::messageToSend_t _messageToSend = nullptr;
        ::notifyHandshake_t _notifyHandshake = nullptr;
        bool _adapter_manages_sync_thread = false;
        ::inject_sync_event_t _inject_action = _inject_sync_event;
        std::thread _sync_thread;
        ::utility::locked_queue<SYNC_EVENT, ::free_Sync_event> sync_evt_q;
        std::mutex mut;

        // private
        std::thread::id sync_thread_id()
        {
            return _sync_thread.get_id();
        }

        // public
        void sync_initialize(
            SyncModes mode,
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake,
            bool adapter_manages_sync_thread)
        {
            _messageToSend = messageToSend;
            _notifyHandshake = notifyHandshake;
            _adapter_manages_sync_thread = adapter_manages_sync_thread;
            set_sync_mode(mode);
            return;
        }

        // public
        void set_sync_mode(SyncModes mode)
        {
//            std::lock_guard<mutex> lock(mut);
            _sync_mode = mode;
            if (_sync_mode == SyncModes::Sync) {
                // init session with inject_sync = process
                // stop sync
                session(release);
                _inject_action = _process_sync_event;
                session(init);
                ::register_sync_callbacks(session(), nullptr, _notifyHandshake, _retrieve_next_sync_event);
                if(!_adapter_manages_sync_thread) {
                    shutdown();
                } else {
                    // The adapter need to shutdown sync thread
                }
            }
            if (_sync_mode == SyncModes::Async) {
                // init session with inject_sync = queue
                // start sync thread
                session(release);
                _inject_action = _inject_sync_event;
                session(init);
                if(!_adapter_manages_sync_thread) {
                    if (!is_sync_running()) {
                        startup<void>(_messageToSend, _notifyHandshake, nullptr, nullptr);
                    }
                } else {
                    // The adapter need to do sync thread start up
                }
            }
            if (_sync_mode == SyncModes::Off) {
                // init sesssion with inject_sync = null
                // stop sync thread
                if(!_adapter_manages_sync_thread) {
                    shutdown();
                } else {
                    // Adapter needs to shutdown sync thread
                }
                session(release);
                _inject_action = _inject_sync_event;
                session(init);
            }
            return;
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

        // public
        ::PEP_SESSION Session::operator()(session_action action)
        {
            std::lock_guard<mutex> lock(mut);

            ::PEP_STATUS status = ::PEP_STATUS_OK;

            switch (action) {
                case release:
                    if (_session.get()) {
                        _session = nullptr;
                    }
                    break;

                case init:
                    if (!_session.get()) {
                        ::PEP_SESSION session_;
                        status = ::init(&session_, _messageToSend, _inject_action, _ensure_passphrase);
                        throw_status(status);
                        _session = SessionPtr{session_, ::release};
                    }
                    break;
                default:
                    status = ::PEP_ILLEGAL_VALUE;
            }

            throw_status(status);
            return _session.get();
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
            if(!_adapter_manages_sync_thread) {
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
