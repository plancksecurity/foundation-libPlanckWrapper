// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_ADAPTER_HH
#define LIBPEPADAPTER_ADAPTER_HH

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <pEp/sync_api.h>
#include "callback_dispatcher.hh"

namespace pEp {

    // throws std::bad_alloc if status==PEP_OUT_OF_MEMORY,
    // throws std::invalid_argument if status==PEP_ILLEGAL_VALUE,
    // throws RuntimeError when 'status' represents another exceptional value.
    void throw_status(::PEP_STATUS status);

    struct RuntimeError : std::runtime_error {
        RuntimeError(const std::string &_text, ::PEP_STATUS _status);
        std::string text;
        ::PEP_STATUS status;
    };

    namespace Adapter {
        // public
        enum class SyncModes
        {
            Sync,
            Async
        };

        int _inject_sync_event(::SYNC_EVENT ev, void *management);
        int _process_sync_event(::SYNC_EVENT ev, void *management);

        ::PEP_STATUS _ensure_passphrase(::PEP_SESSION session, const char *fpr);

        void start_sync();

        template<class T = void>
        void startup(
            T *obj = nullptr,
            std::function<void(T *)> _startup = nullptr,
            std::function<void(T *)> _shutdown = nullptr);

        // returns 'true' when called from the "sync" thread, 'false' otherwise.
        bool on_sync_thread();

        // returns the thread id of the sync thread
        std::thread::id sync_thread_id();


        // The thread-local pEp-session
        // CAVEAT: there is a default constructor Sesssion(),
        // BUT
        // the session object needs to be initialized in order to be usable.

        // TODO: remove initialize() and do that in the ctor's
        // remove release() and do that in the destructor

        class Session {
        public:
            Session();
            Session(const Session &) = delete;
            Session(const Session &&) = delete;
            Session operator=(const Session &) = delete;
            Session operator=(const Session &&) = delete;
            ~Session() = default;
            // Initialize()
            // Initializes the session and registers the CallbackDispatcher as callbacks
            //
            // SyncModes sync_mode:
            // * Sync:
            // Unbuffered, synchronous execution model. no pEp-sync-thread will be created, or should be created.
            // The sync-events generated by the engine will be re-injected into the engine straight away.
            // *  ASync:
            // Buffered (queue), asynchronous execution model. A pEp-sync-thread will be created, or should be created.
            // The sync-events generated by the engine are being put into a libpEpAdapter managed queue.
            // The sync-thread will process the events in the queue and inject it into the state-machine in the engine
            //
            // bool adapter_manages_sync_thread:
            // * true: libpEpAdapter will not manage the sync thread, the adapter impl will have to implement it.
            // * false: libpEpAdapter will manage the sync thread
            //
            // TODO:
            // CAUTION: This call may result in a partially initialized session,
            // If there are any problems with register_sync_callbacks()
            // (e.g. due to no own identities yet)
            // it will still succeed.
            // BUT
            // * Sync will not work
            // * Group Encryption will not work
            // TODO: This needs to be resolved in the engine, by creating a
            //  new func register_callbacks() that is not sync specific,
            //  and move the sync-checks to "start-sync()".
            // Current workaround: a warning is printed out in this case
            void initialize(
                SyncModes sync_mode = SyncModes::Async,
                bool adapter_manages_sync_thread = false);

            // initialize()
            // Same as the initialize() method above, but you can specify arbitrary callbacks
            // to be registered.
            void initialize(
                SyncModes sync_mode,
                bool adapter_manages_sync_thread,
                ::messageToSend_t messageToSend,
                ::notifyHandshake_t notifyHandshake);

            // re-creates the session using same values
            void refresh();

            void release();
            PEP_SESSION operator()();

            SyncModes _sync_mode;
            ::messageToSend_t _messageToSend;
            ::notifyHandshake_t _notifyHandshake;
            bool _adapter_manages_sync_thread;
            ::inject_sync_event_t _inject_action;

        private:
            using SessionPtr = std::unique_ptr<_pEpSession, std::function<void(PEP_SESSION)>>;

            void _init(
                ::messageToSend_t messageToSend,
                ::notifyHandshake_t notifyHandshake,
                SyncModes sync_mode,
                bool adapter_manages_sync_thread);

            SessionPtr _session = nullptr;
        };

        extern thread_local Session session;

        // only injects a NULL event into sync_event_queue
        // Use this if adapter_manages_sync_thread
        // Inject first, then join your thread.
        void inject_sync_shutdown();

        // injects a NULL event into sync_event_queue to denote sync thread to
        // shutdown, and joins & removes the sync thread
        void stop_sync();

        bool is_sync_running();
        bool in_shutdown();
    } // namespace Adapter
} // namespace pEp

#include "Adapter.hxx"

#endif // LIBPEPADAPTER_ADAPTER_HH