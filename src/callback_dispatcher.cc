// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "callback_dispatcher.hh"
#include "passphrase_cache.hh"
#include <stdexcept>
#include <cassert>
#include <utility>
#include "Adapter.hh"

pEp::CallbackDispatcher pEp::callback_dispatcher;

namespace pEp {
    PEP_STATUS CallbackDispatcher::messageToSend(::message *msg)
    {
        return callback_dispatcher._messageToSend(msg);
    }

    PEP_STATUS CallbackDispatcher::notifyHandshake(
        ::pEp_identity *me,
        ::pEp_identity *partner,
        ::sync_handshake_signal signal)
    {
        return callback_dispatcher._notifyHandshake(me, partner, signal);
    }

    void CallbackDispatcher::add(
        ::messageToSend_t messageToSend,
        ::notifyHandshake_t notifyHandshake,
        proc on_startup,
        proc shutdown)
    {
        assert(messageToSend);
        if (!messageToSend) {
            throw std::invalid_argument("messageToSend must be set");
        }

        targets.push_back({messageToSend, notifyHandshake, std::move(on_startup), std::move(shutdown)});
    }

    void CallbackDispatcher::remove(::messageToSend_t messageToSend)
    {
        assert(messageToSend);
        if (!messageToSend) {
            throw std::invalid_argument("messageToSend argument needed");
        }

        for (auto target = targets.begin(); target != targets.end(); ++target) {
            if (target->messageToSend == messageToSend) {
                targets.erase(target);
                break;
            }
        }

        if (targets.empty()) {
            Adapter::stop_sync();
        }
    }

    void CallbackDispatcher::on_startup()
    {
        for (const auto& target : targets) {
            if (target.on_startup) {
                target.on_startup();
            }
        }
    }

    void CallbackDispatcher::on_shutdown()
    {
        for (const auto& target : targets) {
            if (target.on_shutdown) {
                target.on_shutdown();
            }
        }
    }

    PEP_STATUS CallbackDispatcher::_messageToSend(::message *msg)
    {
        if (Adapter::on_sync_thread() && !msg) {
            semaphore.try_wait();

            if (Adapter::in_shutdown()) {
                return PEP_SYNC_NO_CHANNEL;
            }

            PEP_STATUS status = PassphraseCache::config_next_passphrase();

            // if the cache has no valid passphrase ask the app
            if (status == PEP_PASSPHRASE_REQUIRED || status == PEP_WRONG_PASSPHRASE) {
                semaphore.stop();
            }

            // the pEp engine must try again
            return status;
        }

        if (Adapter::on_sync_thread()) {
            // a passphrase worked, reset passphrase_cache iterator
            PassphraseCache::config_next_passphrase(true);
        }

        for (const auto& target : targets) {
            ::message *_msg = nullptr;
            if (msg) {
                _msg = ::message_dup(msg);
                if (!_msg) {
                    return PEP_OUT_OF_MEMORY;
                }
            }
            assert(target.messageToSend);
            target.messageToSend(_msg);
        }

        return PEP_STATUS_OK;
    }

    PEP_STATUS CallbackDispatcher::_notifyHandshake(
        ::pEp_identity *me,
        ::pEp_identity *partner,
        ::sync_handshake_signal signal)
    {
        for (const auto& target : targets) {
            if (target.notifyHandshake != nullptr) {
                ::pEp_identity *_me = nullptr;
                if (me) {
                    _me = ::identity_dup(me);
                    if (!_me) {
                        return PEP_OUT_OF_MEMORY;
                    }
                }
                ::pEp_identity *_partner = nullptr;
                if (partner) {
                    _partner = ::identity_dup(partner);
                    if (!_partner) {
                        free_identity(_me);
                        return PEP_OUT_OF_MEMORY;
                    }
                }
                target.notifyHandshake(_me, _partner, signal);
            }
        }

        return PEP_STATUS_OK;
    }
}; // namespace pEp
