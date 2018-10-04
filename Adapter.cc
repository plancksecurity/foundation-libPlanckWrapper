// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "Adapter.hh"
#include <sstream>
#include <iomanip>
#include <assert.h>

namespace pEp {
    using namespace std;

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

        stringstream build;
        build << setfill('0') << "p≡p 0x" << setw(4) << hex << status;
        throw runtime_error(build.str());
    }

    messageToSend_t Adapter::_messageToSend = nullptr;
    notifyHandshake_t Adapter::_notifyHandshake = nullptr;
    std::thread *_sync_thread = nullptr;

    Adapter::Adapter(messageToSend_t messageToSend,
            notifyHandshake_t notifyHandshake, void *obj)
    {
        if (messageToSend)
            _messageToSend = messageToSend;

        if (notifyHandshake)
            _notifyHandshake = notifyHandshake;

        PEP_SESSION _session = session();

        {
            lock_guard<mutex> lock(mtx());

            if (!_sync_thread) {
                _sync_thread = new thread(sync_thread, obj);
                _sync_thread->detach();
            }
        }
    }

    PEP_SESSION Adapter::session(session_action action)
    {
        lock_guard<mutex> lock(mtx());

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

    void Adapter::shutdown()
    {
        _inject_sync_event(nullptr, nullptr);
    }

    int Adapter::_inject_sync_event(SYNC_EVENT ev, void *management)
    {
        if (is_sync_thread(session())) {
            PEP_STATUS status = do_sync_protocol_step(session(), nullptr, ev);
            return status == PEP_STATUS_OK ? 0 : 1;
        }

        try {
            queue().push_front(ev);
        }
        catch (exception&) {
            return 1;
        }
        return 0;
    }

    SYNC_EVENT Adapter::_retrieve_next_sync_event(void *management, time_t threshold)
    {
        time_t started = time(nullptr);
        bool timeout = false;

        while (queue().empty()) {
            int i = 0;
            ++i;
            if (i > 10) {
                if (time(nullptr) > started + threshold) {
                    timeout = true;
                    break;
                }
                i = 0;
            }
            nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);
        }

        if (timeout)
            return new_sync_timeout_event();

        return queue().pop_front();
    }

    void Adapter::sync_thread(void *obj)
    {
        PEP_STATUS status = register_sync_callbacks(session(), nullptr,
                _notifyHandshake, _retrieve_next_sync_event);
        throw_status(status);

        do_sync_protocol(session(), obj);
        unregister_sync_callbacks(session());

        session(release);
        _sync_thread = nullptr;
    }
}

