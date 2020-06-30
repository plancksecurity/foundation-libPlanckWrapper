#pragma once

#include <vector>
#include "Adapter.hh"

namespace pEp {
    class CallbackDispatcher {
        struct callbacks {
            ::messageToSend_t messageToSend;
            ::notifyHandshake_t notifyHandshake;
        };

        std::vector<callbacks> targets;

    public:
        static PEP_STATUS messageToSend(::message *msg);
        static PEP_STATUS notifyHandshake(::pEp_identity *me,
                ::pEp_identity *partner, ::sync_handshake_signal signal);

        void add(::messageToSend_t messageToSend,
                ::notifyHandshake_t notifyHandshake);
        void remove(::messageToSend_t messageToSend);

    protected:
        PEP_STATUS _messageToSend(::message *msg);
        PEP_STATUS _notifyHandshake(::pEp_identity *me,
                ::pEp_identity *partner, ::sync_handshake_signal signal);
    };

    extern CallbackDispatcher dispatcher;
};

