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
        extern std::mutex m;

        ::SYNC_EVENT _retrieve_next_sync_event(void *management, unsigned threshold);

        static std::exception_ptr _ex;
        static std::atomic_bool register_done{false};

        template<class T>
        void sync_thread(T *obj, function<void(T *)> _startup, function<void(T *)> _shutdown)
        {
            pEpLog("called");
            _ex = nullptr;
            assert(_messageToSend);
            assert(_notifyHandshake);
            if (obj && _startup) {
                _startup(obj);
            }

            pEpLog("creating session");
            session();

            {
                // TODO: Do we need to use a passphraseWrap here???
                pEpLog("register_sync_callbacks()");
                ::PEP_STATUS status = ::register_sync_callbacks(
                    session(),
                    nullptr,
                    _notifyHandshake,
                    _retrieve_next_sync_event);

                pEpLog("register_sync_callbacks() return:" << status);
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
            ::do_sync_protocol(session(), (void *)obj);
            pEpLog("sync protocol loop ended");
            unregister_sync_callbacks(session());

            session(release);

            if (obj && _shutdown) {
                _shutdown(obj);
            }
        }

        template<class T>
        void startup(
            messageToSend_t messageToSend,
            notifyHandshake_t notifyHandshake,
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
            pEpLog("creating session");
            session();

            if (!_sync_thread.joinable()) {
                register_done.store(false);
                pEpLog("creating sync-thread");

                _sync_thread = std::thread(sync_thread<T>, obj, _startup, _shutdown);
                while (register_done.load() == false) {
                    pEpLog("waiting for sync-thread to init...");
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                if (_ex) {
                    pEpLog("exception pending, rethrowing");
                    std::rethrow_exception(_ex);
                }
            }
        }
    } // namespace Adapter
} // namespace pEp

#endif // LIBPEPADAPTER_ADAPTER_HXX