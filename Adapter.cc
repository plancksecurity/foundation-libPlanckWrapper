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

        throw RuntimeError(status_to_string(status), status);
    }

    RuntimeError::RuntimeError(string _text, PEP_STATUS _status)
        : runtime_error(_text.c_str()), text(_text),  status(_status)
    {

    }

    namespace Adapter {
        messageToSend_t _messageToSend = nullptr;
        notifyHandshake_t _notifyHandshake = nullptr;
        messageToSend_t _messageToSend_sync = nullptr;
        notifyHandshake_t _notifyHandshake_sync = nullptr;
        std::thread *_sync_thread = nullptr;

        ::utility::locked_queue< SYNC_EVENT > q;
        std::mutex m;

        int _inject_sync_event(SYNC_EVENT ev, void *management)
        {
            if (is_sync_thread(session())) {
                PEP_STATUS status = do_sync_protocol_step(session(), nullptr, ev);
                return status == PEP_STATUS_OK ? 0 : 1;
            }

            try {
                q.push_front(ev);
            }
            catch (exception&) {
                return 1;
            }
            return 0;
        }

        SYNC_EVENT _retrieve_next_sync_event(void *management, time_t threshold)
        {
            time_t started = time(nullptr);
            bool timeout = false;

            while (q.empty()) {
                int i = 0;
                ++i;
                if (i > 10) {
                    if (time(nullptr) > started + threshold) {
                        timeout = true;
                        break;
                    }
                    i = 0;
                }
#ifdef WIN32
                const xtime xt[] = { { 0, 100000000L } };
                _Thrd_sleep(xt);
#else
                nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);
#endif
            }

            if (timeout)
                return new_sync_timeout_event();

            return q.pop_front();
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
            lock_guard<mutex> lock(m);
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
                    if (!_session) {
                        if (in_sync && _messageToSend_sync)
                            status = ::init(&_session, _messageToSend_sync, _inject_sync_event);
                        else
                            status = ::init(&_session, _messageToSend, _inject_sync_event);
                    }
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
                _sync_thread->join();
                delete _sync_thread;
                _sync_thread = nullptr;
            }
            session(release);
        }
    }
}
