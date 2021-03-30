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
            Off,
            Sync,
            Async
        };

        void sync_initialize(
            SyncModes mode,
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake,
            bool adapter_manages_sync_thread);

        void set_sync_mode(SyncModes mode);

        int _inject_sync_event(::SYNC_EVENT ev, void *management);
        int _process_sync_event(::SYNC_EVENT ev, void *management);

        ::PEP_STATUS _ensure_passphrase(::PEP_SESSION session, const char *fpr);

        template<class T = void>
        void startup(
            messageToSend_t messageToSend,
            notifyHandshake_t notifyHandshake,
            T *obj = nullptr,
            std::function<void(T *)> _startup = nullptr,
            std::function<void(T *)> _shutdown = nullptr);

        // returns 'true' when called from the "sync" thread, 'false' otherwise.
        bool on_sync_thread();

        // returns the thread id of the sync thread
        std::thread::id sync_thread_id();

        enum session_action
        {
            init,
            release,
        };

        class Session {
            using SessionPtr = std::unique_ptr<_pEpSession, std::function<void(PEP_SESSION)>>;
            SessionPtr _session = nullptr;

        public:
            PEP_SESSION operator()(session_action action = init);
        };

        extern thread_local Session session;

        // injects a NULL event into sync_event_queue to denote sync thread to
        // shutdown, and joins & removes the sync thread
        void shutdown();

        bool is_sync_running();
        bool in_shutdown();
    } // namespace Adapter
} // namespace pEp

#include "Adapter.hxx"

#endif // LIBPEPADAPTER_ADAPTER_HH