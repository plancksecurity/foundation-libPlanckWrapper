// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_CALLBACK_DISPATCHER_HH
#define LIBPEPADAPTER_CALLBACK_DISPATCHER_HH

#include <vector>
#include <functional>
#include <mutex>

#include <pEp/Semaphore.hh>
#include "passphrase_cache.hh"
#include <pEp/sync_api.h>
namespace pEp {
    // use this class when implementing a desktop adapter
    // register different interfaces with add()
    // then use CallbackDispatcher::start_sync() to start Sync
    // and CallbackDispatcher::stop_sync() to stop Sync

    class CallbackDispatcher {
        using proc = std::function<void()>;

        struct callbacks {
            ::messageToSend_t messageToSend;
            ::notifyHandshake_t notifyHandshake;
            proc on_startup;
            proc on_shutdown;
        };
        std::vector<callbacks> targets;
        Semaphore semaphore;

    public:
        void add(
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake,
            proc on_startup = nullptr,
            proc on_shutdown = nullptr);
        void remove(::messageToSend_t messageToSend);

        static PEP_STATUS messageToSend(::message *msg);
        static PEP_STATUS notifyHandshake(
            ::pEp_identity *me,
            ::pEp_identity *partner,
            ::sync_handshake_signal signal);

        void on_startup();
        void on_shutdown();
    protected:

        PEP_STATUS _messageToSend(::message *msg);
        PEP_STATUS _notifyHandshake(
            ::pEp_identity *me,
            ::pEp_identity *partner,
            ::sync_handshake_signal signal);

        friend const char *PassphraseCache::add(const std::string &passphrase);
    };

    extern CallbackDispatcher callback_dispatcher;
} // namespace pEp

#endif // LIBPEPADAPTER_CALLBACK_DISPATCHER_HH
