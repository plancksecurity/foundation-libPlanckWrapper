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
    void throw_status(PEP_STATUS status)
    {
        if (status == PEP_STATUS_OK)
            return;
        if (status >= 0x400 && status <= 0x4ff)
            return;
        if (status == PEP_STATEMACHINE_CANNOT_SEND)
            return;
        if (status == PEP_OUT_OF_MEMORY)
            throw bad_alloc();
        if (status == PEP_ILLEGAL_VALUE)
            throw invalid_argument("illegal value");

		string _status = status_to_string(status);
        throw RuntimeError(_status, status);
    }

    RuntimeError::RuntimeError(const std::string& _text, PEP_STATUS _status)
        : std::runtime_error(_text.c_str()), text(_text),  status(_status)
    {

    }

    namespace Adapter {
        messageToSend_t _messageToSend = nullptr;
        notifyHandshake_t _notifyHandshake = nullptr;
        std::thread _sync_thread;

        ::utility::locked_queue< SYNC_EVENT, ::free_Sync_event > sync_evt_q;
        std::mutex m;

        std::thread::id sync_thread_id()
        {
            return _sync_thread.get_id();
        }

        int _inject_sync_event(SYNC_EVENT ev, void *management)
        {
            try {
                if (ev == nullptr) {
                    sync_evt_q.clear();
                    sync_evt_q.push_back(ev);
                }
                else {
                    sync_evt_q.push_front(ev);
                }
            }
            catch (exception&) {
                return 1;
            }
            return 0;
        }

        PEP_STATUS _ensure_passphrase(PEP_SESSION session, const char *fpr)
        {
            return passphrase_cache.ensure_passphrase(session, fpr);
        }

        // threshold: max waiting time in seconds
        SYNC_EVENT _retrieve_next_sync_event(void *management, unsigned threshold)
        {
            SYNC_EVENT syncEvent = nullptr;
            const bool success = sync_evt_q.try_pop_front(syncEvent, std::chrono::seconds(threshold));

            if (!success)
                return new_sync_timeout_event();

            return syncEvent;
        }

        bool on_sync_thread()
        {
            return _sync_thread.get_id() == this_thread::get_id();
        }

        PEP_SESSION Session::operator()(session_action action)
        {
            std::lock_guard<mutex> lock(m);
            bool in_sync = on_sync_thread();

            PEP_STATUS status = PEP_STATUS_OK;

            switch (action) {
                case release:
                    if (_session.get())
                        _session = nullptr;
                    break;

                case init:
                    if (!_session.get()) {
                        PEP_SESSION session_;
                        status = ::init(&session_, _messageToSend, _inject_sync_event, _ensure_passphrase);
                        throw_status(status);
                        _session = SessionPtr{session_, ::release};
                    }
                    break;

                default:
                    status = PEP_ILLEGAL_VALUE;
            }

            throw_status(status);
            return _session.get();
        }

        void shutdown()
        {
        	pEpLog("called");
            if (_sync_thread.joinable()) {
            	pEpLog("sync_is_running - injecting null event");
                _inject_sync_event(nullptr, nullptr);
                _sync_thread.join();
            }
        }

        bool is_sync_running()
        {
            return _sync_thread.joinable();
        }

        bool in_shutdown()
        {
            SYNC_EVENT ev;
            try {
                ev = sync_evt_q.back();
            }
            catch (std::underflow_error&) {
                return false;
            }
            if (ev)
                return false;
            else
                return true;
       }
    }
}
