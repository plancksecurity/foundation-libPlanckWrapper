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
    namespace Adapter {
        // Used to define if sync events are being processes
        // sync (single-threaded) or async (multi-threaded)
        enum class SyncModes {
            Sync,
            Async
        };

        // pEp-Session manager
        // handles lifecycle (create/destroy) and configuratiin of a PEP_SESSION
        // CAVEAT: there is a default constructor Sesssion(),
        // BUT
        // the session object needs to be initialized in order to be usable.
        // The initialization defines the session configuration for all sessions that
        // are being created in the lifetime of a process
        // A Session object is not copyable/assignable

        class Session {
        public:
            Session();
            Session(const Session &) = delete;
            Session(const Session &&) = delete;
            Session operator=(const Session &) = delete;
            Session operator=(const Session &&) = delete;
            ~Session() = default;

            // Initialize()
            // Initializes the session and registers the CallbackDispatcher's functions for callbacks
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
            static void initialize(
                SyncModes sync_mode = SyncModes::Async,
                bool adapter_manages_sync_thread = false);

            // initialize()
            // Same as the initialize() method above, but you can specify arbitrary callbacks
            // to be registered.
            static void initialize(
                SyncModes sync_mode,
                bool adapter_manages_sync_thread,
                ::messageToSend_t messageToSend,
                ::notifyHandshake_t notifyHandshake);

            void release();

            // returns the managed PEP_SESSION
            PEP_SESSION operator()();

            static bool adapter_manages_sync_thread();

        private:
            using SessionPtr = std::unique_ptr<_pEpSession, std::function<void(PEP_SESSION)>>;

            static void _init(
                ::messageToSend_t messageToSend,
                ::notifyHandshake_t notifyHandshake,
                SyncModes sync_mode,
                bool adapter_manages_sync_thread);

            // creates the session
            void _new();

            SessionPtr _session = nullptr;

            static SyncModes _cfg_sync_mode;
            static bool _cfg_adapter_manages_sync_thread;
            static ::messageToSend_t _cfg_cb_messageToSend;
            static ::notifyHandshake_t _cfg_cb_notifyHandshake;
            static ::inject_sync_event_t _cfg_cb_inject_sync_event;

            static bool _is_initialized;
        };

        // the single thread-local instance of class Session
        extern thread_local Session session;

        // ---------------------------------------------------------------------------------------
        // SYNC
        // ---------------------------------------------------------------------------------------
        int _cb_inject_sync_event_do_sync_protocol_step(::SYNC_EVENT ev, void *management);
        int _cb_inject_sync_event_enqueue_sync_event(::SYNC_EVENT ev, void *management);


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

        // only injects a NULL event into sync_event_queue
        // Use this if adapter_manages_sync_thread
        // Inject first, then join your thread.
        void inject_sync_shutdown();

        // injects a NULL event into sync_event_queue to denote sync thread to
        // shutdown, and joins & removes the sync thread
        void stop_sync();

        bool is_sync_running();
        bool in_shutdown();

        ::PEP_STATUS _ensure_passphrase(::PEP_SESSION session, const char *fpr);
    } // namespace Adapter

    // throws std::bad_alloc if status==PEP_OUT_OF_MEMORY,
    // throws std::invalid_argument if status==PEP_ILLEGAL_VALUE,
    // throws RuntimeError when 'status' represents another exceptional value.
    void throw_status(::PEP_STATUS status);

    struct RuntimeError : std::runtime_error {
        RuntimeError(const std::string &_text, ::PEP_STATUS _status);
        std::string text;
        ::PEP_STATUS status;
    };
} // namespace pEp

#include "Adapter.hxx"

#endif // LIBPEPADAPTER_ADAPTER_HH