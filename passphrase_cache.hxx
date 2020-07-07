#include "passphrase_cache.hh"

extern pEp::CallbackDispatcher callback_dispatcher;

namespace pEp {
    template<typename... A> PEP_STATUS PassphraseCache::api(
            PEP_STATUS f(PEP_SESSION, A...), PEP_SESSION session, A... a)
    {
        PEP_STATUS status;

        do {
            if (synchronous) {
                callback_dispatcher.semaphore.try_wait();
            }

            for_each_passphrase([&](std::string passphrase) {
                status = ::config_passphrase(session, passphrase.c_str());
                if (status)
                    return true;

                status = f(session, a...);
                return status != PEP_PASSPHRASE_REQUIRED &&
                        status != PEP_WRONG_PASSPHRASE;
            });

            if (synchronous) {
                CallbackDispatcher::notifyHandshake(nullptr, nullptr, SYNC_PASSPHRASE_REQUIRED);
                callback_dispatcher.semaphore.stop();
            }
        } while (synchronous);

        return status;
    }
};

