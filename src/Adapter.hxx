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
        void sync_thread(Session *rhs, T *obj, function<void(T *)> _startup, function<void(T *)> _shutdown)
        {
            pEpLog("called");
            _ex = nullptr;

            // 1. Execute registered startup function
            if (obj && _startup) {
                _startup(obj);
            }

            pEpLog("creating session for the sync thread");
            // 2. Create session for the sync thread
            // 3. register_sync_callbacks() (in session.initialize())
            try {
                session.initialize(
                    rhs->_sync_mode,
                    rhs->_adapter_manages_sync_thread,
                    rhs->_messageToSend,
                    rhs->_notifyHandshake);
                register_done.store(true);
            } catch (...) {
                _ex = std::current_exception();
                register_done.store(true);
                return;
            }

            pEpLog("sync protocol loop started");
            // 4. Enter Sync Event Dispatching Loop (do_sync_protocol())
            ::do_sync_protocol(session(), (void *)obj);
            pEpLog("sync protocol loop ended");

            // 5. unregister_sync_callbacks()
            unregister_sync_callbacks(session());

            // 6. Release the session
            session.release();

            // 7. Execute registered shutdown function
            if (obj && _shutdown) {
                _shutdown(obj);
            }
        }

        /*
         * Sync Thread Startup
         * 1. throw if main thread session is not initialized
         * 2. Start the sync thread
         * 3. Defer execution until sync thread register_sync_callbacks() has returned
         * 4. Throw pending exception from the sync thread
         */
        // private
        template<class T>
        void startup(T *obj, std::function<void(T *)> _startup, std::function<void(T *)> _shutdown)
        {
            pEpLog("called");
            // refresh the session
            // due to partially initialized session, see session.initialize()
            session.refresh();

            if (!_sync_thread.joinable()) {
                register_done.store(false);
                pEpLog("creating sync-thread");
                // 2. Start the sync thread
                _sync_thread = std::thread(sync_thread<T>, &session, obj, _startup, _shutdown);
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