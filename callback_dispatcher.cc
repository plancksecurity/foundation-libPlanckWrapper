#include "callback_dispatcher.hh"
#include <stdexcept>
#include <cassert>

pEp::CallbackDispatcher pEp::callback_dispatcher;

namespace pEp {
    bool CallbackDispatcher::once = true;

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

    void CallbackDispatcher::add(
            ::messageToSend_t messageToSend,
            ::notifyHandshake_t notifyHandshake,
            proc on_startup,
            proc shutdown
        )
    {
        assert(messageToSend);
        if (!messageToSend)
            throw std::invalid_argument("messageToSend must be set");

        targets.push_back({messageToSend, notifyHandshake, on_startup, shutdown});
    }

    void CallbackDispatcher::remove(::messageToSend_t messageToSend)
    {
        assert(messageToSend);
        if (!messageToSend)
            throw std::invalid_argument("messageToSend argument needed");

        for (auto target = targets.begin(); target != targets.end(); ++target) {
            if (target->messageToSend == messageToSend) {
                targets.erase(target);
                break;
            }
        }
        if (targets.empty())
            stop_sync();
    }

    void CallbackDispatcher::on_startup()
    {
        for (auto target : targets) {
            if (target.on_startup)
                target.on_startup();
        }
    }

    void CallbackDispatcher::on_shutdown()
    {
        for (auto target : targets) {
            if (target.on_shutdown)
                target.on_shutdown();
        }
    }

    void CallbackDispatcher::start_sync()
    {
        pEp::Adapter::startup<CallbackDispatcher>(CallbackDispatcher::messageToSend,
                CallbackDispatcher::notifyHandshake, &callback_dispatcher,
                &CallbackDispatcher::on_startup,
                &CallbackDispatcher::on_shutdown);

        for (auto target : callback_dispatcher.targets) {
            if (target.notifyHandshake)
                target.notifyHandshake(nullptr, nullptr, SYNC_NOTIFY_START);
        }
    }

    void CallbackDispatcher::stop_sync()
    {
        pEp::Adapter::shutdown();

        for (auto target : callback_dispatcher.targets) {
            if (target.notifyHandshake)
                target.notifyHandshake(nullptr, nullptr, SYNC_NOTIFY_STOP);
        }
    }

    PEP_STATUS CallbackDispatcher::_messageToSend(::message *msg)
    {
        for (auto target : targets) {
            auto _msg = ::message_dup(msg);
            if (!_msg)
                return PEP_OUT_OF_MEMORY;

            assert(target.messageToSend);
            target.messageToSend(_msg);
        }

        return PEP_STATUS_OK;
    }

    PEP_STATUS CallbackDispatcher::_notifyHandshake(::pEp_identity *me,
            ::pEp_identity *partner, ::sync_handshake_signal signal)
    {
        for (auto target : targets) {
            if (target.notifyHandshake) {
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
        }

        return PEP_STATUS_OK;
    }
};

