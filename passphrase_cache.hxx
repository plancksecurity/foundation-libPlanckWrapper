#pragma once
#include "passphrase_cache.hh"

namespace pEp {
    template<typename... A> PEP_STATUS PassphraseCache::api(
            PEP_STATUS f(PEP_SESSION, A...), PEP_SESSION session, A... a)
    {
        PEP_STATUS status;

        for_each_passphrase([&](std::string passphrase) {
            ::config_passphrase(session, passphrase.c_str());
            status = f(session, a...);
            return status != PEP_PASSPHRASE_REQUIRED &&
                    status != PEP_WRONG_PASSPHRASE;
        });

        return status;
    }
};

