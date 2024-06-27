// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include <cassert>
#include "Adapter.hh"
#include "passphrase_cache.hh"
#include "callback_dispatcher.hh"

pEp::PassphraseCache pEp::passphrase_cache;

namespace pEp {
    PassphraseCache::cache_entry::cache_entry(const std::string& p, time_point t) :
        passphrase{ p, 0, PassphraseCache::cache_entry::max_len }, tp{ t }
    {
    }

    PassphraseCache::cache_entry::cache_entry(const std::string& e, const std::string& p, time_point t) :
        account_email{ e, 0, PassphraseCache::cache_entry::max_len },
        passphrase{ p, 0, PassphraseCache::cache_entry::max_len }, tp{ t }
    {
    }

    PassphraseCache::PassphraseCache(size_t max_size, duration timeout) :
        _max_size{ max_size }, _timeout{ timeout }, _which(_cache.end()), first_time(true)
    {
    }

    PassphraseCache::PassphraseCache(const PassphraseCache& second) :
        _cache{ second._cache }, _max_size{ second._max_size }, _timeout{ second._timeout },
        _stored{ second._stored }, _which(_cache.end()), first_time(true)
    {
    }

    PassphraseCache& PassphraseCache::operator=(const PassphraseCache& second)
    {
        _cache = second._cache;
        _max_size = second._max_size;
        _timeout = second._timeout;
        _which = _cache.end();
        return *this;
    }

    const char *PassphraseCache::add(const cache_entry& entry)
    {
        const char* result = nullptr;
        std::lock_guard<std::mutex> lock(_mtx);

        auto found = find(_cache.begin(), _cache.end(), entry);

        if (found != _cache.end()) {
            refresh(found);
            result = found->passphrase.c_str();
        } else {
            _cache.push_back(entry);
            auto back = _cache.end();
            assert(!_cache.empty());
            result = (--back)->passphrase.c_str();
        }

        return result;
    }

    const char* PassphraseCache::add(const std::string& passphrase)
    {
        if (!passphrase.empty()) {
            auto entry = cache_entry(passphrase, clock::now());
            const char* result = add(entry);
            callback_dispatcher.semaphore.go();
            return result;
        }

        static const char* empty = "";
        return empty;
    }

    const char* PassphraseCache::add(const std::string& account_email, const std::string& passphrase)
    {
        if (!passphrase.empty()) {
            auto entry = cache_entry(account_email, passphrase, clock::now());
            const char* result = add(entry);
            callback_dispatcher.semaphore.go();
            return result;
        }

        static const char* empty = "";
        return empty;
    }

    const char* PassphraseCache::add_stored(const std::string& passphrase)
    {
        std::lock_guard<std::mutex> lock(_stored_mtx);
        _stored = passphrase;
        return _stored.c_str();
    }

    bool PassphraseCache::for_each_passphrase(const passphrase_callee& callee)
    {
        if (callee(std::string())) {
            return true;
        }

        {
            std::lock_guard<std::mutex> lock(_stored_mtx);
            if (!_stored.empty() && callee(_stored)) {
                return true;
            }
        }

        {
            std::lock_guard<std::mutex> lock(_mtx);

            for (auto entry = _cache.begin(); entry != _cache.end(); ++entry) {
                if (callee(entry->passphrase)) {
                    refresh(entry);
                    return true;
                }
            }
        }

        return false;
    }

    void PassphraseCache::refresh(cache::iterator entry)
    {
        entry->tp = clock::now();
        _cache.splice(_cache.end(), _cache, entry);
    }

    const char* PassphraseCache::latest_passphrase(PassphraseCache& c)
    {
        if (c.first_time) {
            c._which = c._cache.end();
            c.first_time = false;
            if (!c._stored.empty()) {
                return c._stored.c_str();
            }
        }

        if (c._cache.empty()) {
            c.first_time = true;
            throw Empty();
        }

        if (c._which == c._cache.begin()) {
            c.first_time = true;
            throw Exhausted();
        }

        --c._which;
        return c._which->passphrase.c_str();
    }

    PEP_STATUS PassphraseCache::config_next_passphrase(bool reset, PEP_SESSION session)
    {
        static pEp::PassphraseCache _copy;
        static bool new_copy = true;

        if (reset) {
            new_copy = true;
            return PEP_STATUS_OK;
        }

        if (new_copy) {
            _copy = passphrase_cache;
            new_copy = false;
        }

        try {
            ::config_passphrase(
                session != nullptr ? session : Adapter::session(),
                latest_passphrase(_copy));
            return PEP_STATUS_OK;
        } catch (pEp::PassphraseCache::Empty&) {
            new_copy = true;
            return PEP_PASSPHRASE_REQUIRED;
        } catch (pEp::PassphraseCache::Exhausted&) {
            new_copy = true;
            return PEP_WRONG_PASSPHRASE;
        }
    }

    PEP_STATUS PassphraseCache::ensure_passphrase(PEP_SESSION session, std::string fpr)
    {
        PEP_STATUS status{ PEP_STATUS_OK };

        for_each_passphrase([&](const std::string& passphrase) {
            status = ::config_passphrase(session, passphrase.c_str());
            if (status != 0) {
                return true;
            }
            status = ::probe_encrypt(session, fpr.c_str());
            return status == PEP_STATUS_OK;
        });

        return status;
    }

    void PassphraseCache::configure_session(PEP_SESSION session)
    {
        // TODO: Testing. Remove.
        ::config_passphrase_for_new_keys(session, true, "uiae2");
        pEp::passphrase_cache.add("uiae1");
        pEp::passphrase_cache.add("deckard@planck.dev", "uiae2");
        pEp::passphrase_cache.add("not_there@planck.dev", "uiae3");
        // End of testing (remove with any testing setup that gets removed)

        std::lock_guard<std::mutex> lock(_mtx);

        stringpair_list_t *account_passphrases = nullptr;

        for (auto entry = _cache.begin(); entry != _cache.end(); ++entry) {
            if (!entry->account_email.empty()) {
                stringpair_t *pair = new_stringpair(entry->account_email.c_str(), entry->passphrase.c_str());
                stringpair_list_t *list = stringpair_list_add(account_passphrases, pair);
                if (!account_passphrases) {
                    account_passphrases = list;
                }
            }
        }

        ::configure_account_passphrases(session, account_passphrases);
    }

} // namespace pEp
