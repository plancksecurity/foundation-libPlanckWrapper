// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PASSPHRASE_CACHE_HXX
#define LIBPEPADAPTER_PASSPHRASE_CACHE_HXX

#include "passphrase_cache.hh"

namespace pEp {
    template<typename... A>
    PEP_STATUS PassphraseCache::api(PEP_STATUS f(PEP_SESSION, A...), PEP_SESSION session, A... a)
    {
        PEP_STATUS status{ PEP_STATUS_OK };

        for_each_passphrase([&](const std::string& passphrase) {
            status = ::config_passphrase(session, passphrase.c_str());
            if (status) {
                return true;
            }

            status = f(session, a...);
            return status != PEP_PASSPHRASE_REQUIRED && status != PEP_WRONG_PASSPHRASE;
        });

        return status;
    }
} // namespace pEp

#endif // LIBPEPADAPTER_PASSPHRASE_CACHE_HXX
