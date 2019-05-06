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

        template< class T > void sync_thread(T *obj, function< void(T *) > _startup, function< void(T *) > _shutdown)
        {
            assert(_messageToSend);
            assert(_notifyHandshake);
            if (obj && _startup)
                _startup(obj);

            session();

            PEP_STATUS status = register_sync_callbacks(session(), nullptr,
                _notifyHandshake, _retrieve_next_sync_event);
            throw_status(status);

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
        {
            if (messageToSend)
                _messageToSend = messageToSend;

            if (notifyHandshake)
                _notifyHandshake = notifyHandshake;

            session();

            {
                std::lock_guard<std::mutex> lock(m);

                if (!_sync_thread)
                    _sync_thread = new std::thread(sync_thread<T>, obj, _startup, _shutdown);
            }
        }
    }
}
