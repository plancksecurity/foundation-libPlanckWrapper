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

        for_each_passphrase([&](const cache_entry entry) {
            status = ::config_passphrase(session, entry.passphrase.c_str());
            if (entry.email == PASSPHRASE_FOR_NEW_KEYS_ENTRY && !entry.passphrase.empty()) { // if this entry exists, app is running in managed environment
                status = ::config_passphrase_for_new_keys(session, true, entry.passphrase.c_str());
            }
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
