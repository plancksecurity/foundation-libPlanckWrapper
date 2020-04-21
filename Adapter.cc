// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "Adapter.hh"
#include <sstream>
#include <iomanip>
#include <assert.h>
#include "status_to_string.hh"
#include "utils.hh"


using namespace std;

namespace pEp {
    void throw_status(PEP_STATUS status)
    {
        //pEpLog("called");
        if (status == PEP_STATUS_OK)
            return;
        if (status >= 0x400 && status <= 0x4ff)
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
        pEpLog("called");
    }

    namespace Adapter {
        messageToSend_t _messageToSend = nullptr;
        notifyHandshake_t _notifyHandshake = nullptr;
        std::thread *_sync_thread = nullptr;

        ::utility::locked_queue< SYNC_EVENT, ::free_Sync_event > sync_q;
        std::mutex mutex_global;

        int _inject_sync_event(SYNC_EVENT ev, void *management)
        {
            pEpLog("called");
            try {
                if (ev == nullptr) {
                    pEpLog("SYNC_EVENT: NULL");
                    sync_q.clear();
                    sync_q.push_back(ev);
                }
                else {
                    pEpLog("SYNC_EVENT:" << ev);
                    sync_q.push_front(ev);
                }
            }
            catch (exception&) {
                pEpErr("Exception");
                return 1;
            }
            if (ev == nullptr) {
                if (!on_sync_thread()) {
                    if(_sync_thread->joinable()) {
                        pEpLog("Waiting for Sync thread to join...");
                        _sync_thread->join();
                        delete _sync_thread;
                        _sync_thread = nullptr;
                        pEpLog("...thread joined");
                        sync_q.clear();
                    } else  {
                        //FATAL
                        pEpLog("FATAL: sync thread not joinable/detached");
                    }
                }
            }
            return 0;
        }

        // threshold: max waiting time in seconds
        SYNC_EVENT _retrieve_next_sync_event(void *management, unsigned threshold)
        {
            pEpLog("called");
            SYNC_EVENT syncEvent = nullptr;
            const bool success = sync_q.try_pop_front(syncEvent, std::chrono::seconds(threshold));

            if (!success) {
                pEpLog("timeout after [sec]: " << threshold);
                return new_sync_timeout_event();
            }

            pEpLog("returning SYNC_EVENT: "  << syncEvent);
            return syncEvent;
        }

        bool on_sync_thread()
        {
            //pEpLog("called");
            if (_sync_thread && _sync_thread->get_id() == this_thread::get_id())
                return true;
            else
                return false;
        }

        PEP_SESSION session(session_action action)
        {
            pEpLog("called");
            std::lock_guard<mutex> lock(mutex_global);
            bool in_sync = on_sync_thread();

            thread_local static PEP_SESSION _session = nullptr;
            PEP_STATUS status = PEP_STATUS_OK;

            switch (action) {
                case release:
                    if (_session) {
                        pEpLog("action = release: releasing session: " << _session);
                        ::release(_session);
                        _session = nullptr;
                    } else {
                        pEpLog("action = release: No session to release");
                    }
                    break;

                case init:
                    if (!_session) {
                        pEpLog("action = init: creating new session");
                        status = ::init(&_session, _messageToSend, _inject_sync_event);
                    }
                    break;

                default:
                    status = PEP_ILLEGAL_VALUE;
            }

            throw_status(status);
            pEpLog("returning session: " << _session);
            return _session;
        }

        void shutdown()
        {
        	pEpLog("called");
            if (_sync_thread) {
            	pEpLog("sync_is_running - injecting null event");
                _inject_sync_event(nullptr, nullptr);
            }
        }

        bool is_sync_running()
        {
            pEpLog("called");
            return _sync_thread != nullptr;
        }
    }
}
