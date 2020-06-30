#include "callback_dispatcher.hh"
#include <stdexcept>
#include <cassert>

namespace pEp {
    bool CallbackDispatcher::once = true;
    CallbackDispatcher callback_dispatcher;

    CallbackDispatcher::CallbackDispatcher()
    {
        if (!once)
            throw std::out_of_range("CallbackDispatcher is a singleton");
        once = false;
    }

    PEP_STATUS CallbackDispatcher::messageToSend(::message *msg)
    {
        return callback_dispatcher._messageToSend(msg);
    }

    PEP_STATUS CallbackDispatcher::notifyHandshake(::pEp_identity *me,
            ::pEp_identity *partner, ::sync_handshake_signal signal)
    {
        return callback_dispatcher._notifyHandshake(me, partner, signal);
    }

    void CallbackDispatcher::add(::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake)
    {
        targets.push_back({messageToSend, notifyHandshake});
    }

    void CallbackDispatcher::remove(::messageToSend_t messageToSend)
    {
        for (auto target = targets.begin(); target != targets.end(); ++target) {
            if (target->messageToSend == messageToSend) {
                targets.erase(target);
                break;
            }
        }
    }

    PEP_STATUS CallbackDispatcher::_messageToSend(::message *msg)
    {
        for (auto target : targets) {
            auto _msg = ::message_dup(msg);
            if (!_msg)
                return PEP_OUT_OF_MEMORY;

            target.messageToSend(_msg);
        }

        return PEP_STATUS_OK;
    }

    PEP_STATUS CallbackDispatcher::_notifyHandshake(::pEp_identity *me,
            ::pEp_identity *partner, ::sync_handshake_signal signal)
    {
        for (auto target : targets) {
            auto _me = ::identity_dup(me);
            if (!_me)
                return PEP_OUT_OF_MEMORY;

            auto _partner = ::identity_dup(partner);
            if (!_partner) {
                free_identity(_me);
                return PEP_OUT_OF_MEMORY;
            }

            target.notifyHandshake(_me, _partner, signal);
        }

        return PEP_STATUS_OK;
    }
};

