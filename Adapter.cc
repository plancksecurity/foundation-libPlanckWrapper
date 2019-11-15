// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "Adapter.hh"
#include <sstream>
#include <iomanip>
#include <assert.h>
#include "status_to_string.hh"

using namespace std;

namespace pEp {
    void throw_status(PEP_STATUS status)
    {
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

    }

    namespace Adapter {
        messageToSend_t _messageToSend = nullptr;
        notifyHandshake_t _notifyHandshake = nullptr;
        std::thread *_sync_thread = nullptr;

        ::utility::locked_queue< SYNC_EVENT, ::free_Sync_event > q;
        std::mutex m;

        int _inject_sync_event(SYNC_EVENT ev, void *management)
        {
            try {
                if (ev == nullptr)
                    q.push_back(ev);
                else
                    q.push_front(ev);
            }
            catch (exception&) {
                return 1;
            }
            if (ev == nullptr) {
                if (_sync_thread)
                    _sync_thread->join();
                q.clear();
            }
            return 0;
        }

        // threshold: max waiting time in seconds
        SYNC_EVENT _retrieve_next_sync_event(void *management, unsigned threshold)
        {
            SYNC_EVENT syncEvent = nullptr;
            const bool success = q.try_pop_front(syncEvent, std::chrono::seconds(threshold));

            if (!success)
                return new_sync_timeout_event();

            return syncEvent;
        }

        bool on_sync_thread()
        {
            if (_sync_thread && _sync_thread->get_id() == this_thread::get_id())
                return true;
            else
                return false;
        }

        PEP_SESSION session(session_action action)
        {
            std::lock_guard<mutex> lock(m);
            bool in_sync = on_sync_thread();

            thread_local static PEP_SESSION _session = nullptr;
            PEP_STATUS status = PEP_STATUS_OK;

            switch (action) {
                case release:
                    if (_session) {
                        ::release(_session);
                        _session = nullptr;
                    }
                    break;

                case init:
                    if (!_session)
                        status = ::init(&_session, _messageToSend, _inject_sync_event);
                    break;

                default:
                    status = PEP_ILLEGAL_VALUE;
            }

            throw_status(status);
            return _session;
        }

        void shutdown()
        {
            if (_sync_thread) {
                _inject_sync_event(nullptr, nullptr);
                delete _sync_thread;
                _sync_thread = nullptr;
            }
            session(release);
        }

        bool is_sync_running()
        {
            return _sync_thread != nullptr;
        }
    }
}
