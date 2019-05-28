#pragma once

#include <thread>
#include "locked_queue.hh"
#include <cassert>

namespace pEp {
    namespace Adapter {
        using std::function;

        extern messageToSend_t _messageToSend;
        extern notifyHandshake_t _notifyHandshake;
        extern std::thread *_sync_thread;

        extern ::utility::locked_queue< SYNC_EVENT > q;
        extern std::mutex m;

        SYNC_EVENT _retrieve_next_sync_event(void *management, unsigned threshold);

        std::exception_ptr _ex;
        bool lpa_register_done = false;

        template< class T > void sync_thread(T *obj, function< void(T *) > _startup, function< void(T *) > _shutdown)
        {
            assert(_messageToSend);
            assert(_notifyHandshake);
            if (obj && _startup)
                _startup(obj);

            session();

            {
                PEP_STATUS status = register_sync_callbacks(session(), nullptr,
                    _notifyHandshake, _retrieve_next_sync_event);
                try {
                    throw_status(status);
                    lpa_register_done = true;
                }
                catch (...) {
                    _ex = std::current_exception();
                    lpa_register_done = true;
                    return;
                }
            }

            do_sync_protocol(session(), (void *)obj);
            unregister_sync_callbacks(session());

            session(release);

            if (obj && _shutdown)
                _shutdown(obj);
        }

        template< class T > void startup(
            messageToSend_t messageToSend,
            notifyHandshake_t notifyHandshake,
            T *obj,
            function< void(T *) > _startup,
            function< void(T *) > _shutdown
        )
            throw (RuntimeError)
        {
            if (messageToSend)
                _messageToSend = messageToSend;

            if (notifyHandshake)
                _notifyHandshake = notifyHandshake;

            session();

            {
                std::lock_guard<std::mutex> lock(m);

                if (!_sync_thread) {
                    lpa_register_done = false;
                    _sync_thread = new std::thread(sync_thread<T>, obj, _startup, _shutdown);
                    while (!lpa_register_done)
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));

                    if (_ex) {
                        _sync_thread = nullptr;
                        std::rethrow_exception(_ex);
                    }
                }
            }
        }
    }
}
