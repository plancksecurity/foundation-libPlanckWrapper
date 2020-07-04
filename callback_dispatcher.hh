#pragma once

#include <vector>
#include <functional>
#include "Adapter.hh"

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

    public:
        void add(
                ::messageToSend_t messageToSend,
                ::notifyHandshake_t notifyHandshake,
                proc on_startup = nullptr,
                proc on_shutdown = nullptr
            );
        void remove(::messageToSend_t messageToSend);

        static void start_sync();
        static void stop_sync();

        static PEP_STATUS messageToSend(::message *msg);
        static PEP_STATUS notifyHandshake(::pEp_identity *me,
                ::pEp_identity *partner, ::sync_handshake_signal signal);
    protected:
        void on_startup();
        void on_shutdown();

        PEP_STATUS _messageToSend(::message *msg);
        PEP_STATUS _notifyHandshake(::pEp_identity *me,
                ::pEp_identity *partner, ::sync_handshake_signal signal);
    };

    extern CallbackDispatcher callback_dispatcher;
};

