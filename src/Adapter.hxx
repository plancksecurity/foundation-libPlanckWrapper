// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_ADAPTER_HXX
#define LIBPEPADAPTER_ADAPTER_HXX

#include <thread>
#include <pEp/locked_queue.hh>
#include <cassert>
#include <pEp/pEpLog.hh>
#include <atomic>

namespace pEp {
    namespace Adapter {
        using std::function;

        extern std::thread _sync_thread;
        extern ::utility::locked_queue<::SYNC_EVENT, ::free_Sync_event> sync_evt_q;

        /*
         * Sync Thread
         * 1. Execute registered startup function
         * 2. Create session for the sync thread (registers: messageToSend, inject_sync_event, ensure_passphrase)
         * 3. Enter Sync Event Processing Loop (do_sync_protocol())
         * 4. unregister_sync_callbacks()
         * 5. Release the session
         * 6. Execute registered shutdown function
         */
        // private
        template<class T>
        void sync_thread(T *obj, function<void(T *)> _startup, function<void(T *)> _shutdown)
        {
            pEpLog("called");

            // 1. Execute registered startup function
            if (obj && _startup) {
                _startup(obj);
            }

            // 2. Create session for the sync thread
            pEpLog("creating session for the sync thread");
            session();

            // 3. Enter Sync Event Processing Loop (do_sync_protocol())
            // this internally calls _retrieve_next_sync_event
            pEpLog("sync protocol loop started");
            ::do_sync_protocol(session());
            pEpLog("sync protocol loop ended");

            // 4. unregister_sync_callbacks()
            unregister_sync_callbacks(session());

            // 5. Release the session
            session.release();

            // 6. Execute registered shutdown function
            if (obj && _shutdown) {
                _shutdown(obj);
            }
        }

        // Sync Thread Startup
        // private
        template<class T>
        void startup(T *obj, std::function<void(T *)> _startup, std::function<void(T *)> _shutdown)
        {
            pEpLog("called");
            if (in_shutdown() || !_sync_thread.joinable()) {
                _sync_thread = std::thread(sync_thread<T>, obj, _startup, _shutdown);
            } else {
                pEpLog("Sync thread already running");
            }
        }
    } // namespace Adapter
} // namespace pEp

#endif //LIBPEPADAPTER_ADAPTER_HXX