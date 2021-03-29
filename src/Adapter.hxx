// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_ADAPTER_HXX
#define LIBPEPADAPTER_ADAPTER_HXX

#include <thread>
#include "locked_queue.hh"
#include <cassert>
#include "pEpLog.hh"
#include <atomic>

namespace pEp {
    namespace Adapter {
        using std::function;

        extern ::messageToSend_t _messageToSend;
        extern ::notifyHandshake_t _notifyHandshake;
        extern std::thread _sync_thread;

        extern ::utility::locked_queue<::SYNC_EVENT, ::free_Sync_event> sync_evt_q;
        extern std::mutex mut;

        ::SYNC_EVENT _retrieve_next_sync_event(void *management, unsigned threshold);

        static std::exception_ptr _ex;
        static std::atomic_bool register_done{false};

        /*
         * Sync Thread
         * 1. Execute registered startup function
         * 2. Create session for the sync thread (registers: messageToSend, inject_sync_event, ensure_passphrase)
         * 3. register_sync_callbacks() (registers: _notifyHandshake, _retrieve_next_sync_event)
         * 4. Enter Sync Event Dispatching Loop (do_sync_protocol())
         * 5. unregister_sync_callbacks()
         * 6. Release the session
         * 7. Execute registered shutdown function
         */
        // private
        template<class T>
        void sync_thread(T *obj, function<void(T *)> _startup, function<void(T *)> _shutdown)
        {
            pEpLog("called");
            _ex = nullptr;
            assert(_messageToSend);
            assert(_notifyHandshake);

            // 1. Execute registered startup function
            if (obj && _startup) {
                _startup(obj);
            }

            pEpLog("creating session for the sync thread");
            // 2. Create session for the sync thread
            session();

            // 3. register_sync_callbacks()
            {
                // TODO: Do we need to use a passphraseWrap here???
                pEpLog("register_sync_callbacks()");
                ::PEP_STATUS status = ::register_sync_callbacks(
                    session(),
                    nullptr,
                    _notifyHandshake,
                    _retrieve_next_sync_event);

                pEpLog("register_sync_callbacks() return:" << status);
                // Convert status into exception and store it
                // set register_done AFTER that
                try {
                    throw_status(status);
                    register_done.store(true);
                } catch (...) {
                    _ex = std::current_exception();
                    register_done.store(true);
                    return;
                }
            }

            pEpLog("sync protocol loop started");
            // 4. Enter Sync Event Dispatching Loop (do_sync_protocol())
            ::do_sync_protocol(session(), (void *)obj);
            pEpLog("sync protocol loop ended");

            // 5. unregister_sync_callbacks()
            unregister_sync_callbacks(session());

            // 6. Release the session
            // TODO: Maybe do that AFTER shutdown?
            session(release);

            // 7. Execute registered shutdown function
            if (obj && _shutdown) {
                _shutdown(obj);
            }
        }

        /*
         * Sync Thread Startup
         * 1. ensure session for the main thread (registers: messageToSend, _queue_sync_event, _ensure_passphrase)
         * 2. Start the sync thread
         * 3. Defer execution until sync thread register_sync_callbacks() has returned
         * 4. Throw pending exception from the sync thread
         */
        // private
        template<class T>
        void startup(
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake,
            T *obj,
            function<void(T *)> _startup,
            function<void(T *)> _shutdown)
        {
            pEpLog("called");
            if (messageToSend) {
                _messageToSend = messageToSend;
            }

            if (notifyHandshake) {
                _notifyHandshake = notifyHandshake;
            }
            pEpLog("ensure session for the main thread");
            // 1. re-initialize session for the main thread (registers: messageToSend, _queue_sync_event, _ensure_passphrase)
            session(release);
            session(init);

            if (!_sync_thread.joinable()) {
                register_done.store(false);
                pEpLog("creating sync-thread");
                // 2. Start the sync thread
                _sync_thread = std::thread(sync_thread<T>, obj, _startup, _shutdown);
                // 3. Defer execution until sync thread register_sync_callbacks() has returned
                while (register_done.load() == false) {
                    pEpLog("waiting for sync-thread to init...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                // 4. Throw pending exception from the sync thread
                if (_ex) {
                    pEpLog("exception pending, rethrowing");
                    std::rethrow_exception(_ex);
                }
            }
        }
    } // namespace Adapter
} // namespace pEp

#endif //LIBPEPADAPTER_ADAPTER_HXX