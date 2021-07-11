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

        class Session {
        public:
            // TODO: needed because libpEpAdapter provides a static instance
            // the session needs to be initialized in order to be usable.
            Session();
            // Init using CallbackDispatcher
            // CAUTION: This may result in a partially initialized session.
            // If there are any problem with register_sync_callbacks(), it will still
            // succeed. (e.g. due to no own identities yet)
            // BUT
            // * Sync will not work
            // * Group Encryption will not work
            // TODO: This needs to be resolved in the engine, new func register_callbacks()
            // that is not sync specific, and move the sync-checks to "start-sync()"
            void initialize(SyncModes sync_mode = SyncModes::Async, bool adapter_manages_sync_thread = false);
            // Arbitrary callbacks
            void initialize(
                SyncModes sync_mode,
                bool adapter_manages_sync_thread,
                ::messageToSend_t messageToSend,
                ::notifyHandshake_t notifyHandshake);

            // re-creates the session using same values
            void refresh();

            // Not copyable
            Session(const Session &) = delete;
            Session operator=(const Session&) = delete;

            void release();
            PEP_SESSION operator()();

            SyncModes _sync_mode;
            ::messageToSend_t _messageToSend;
            ::notifyHandshake_t _notifyHandshake;
            bool _adapter_manages_sync_thread;
            ::inject_sync_event_t _inject_action;

        private:
            void _init(
                ::messageToSend_t messageToSend,
                ::notifyHandshake_t notifyHandshake,
                SyncModes sync_mode,
                bool adapter_manages_sync_thread);

            using SessionPtr = std::unique_ptr<_pEpSession, std::function<void(PEP_SESSION)>>;
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