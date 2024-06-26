// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include <cassert>
#include "Adapter.hh"
#include "passphrase_cache.hh"
#include "callback_dispatcher.hh"

pEp::PassphraseCache pEp::passphrase_cache;

namespace pEp {
    const std::string pEp::PassphraseCache::PASSPHRASE_FOR_NEW_KEYS_ENTRY = "PASSPHRASE_FOR_NEW_KEYS_ENTRY";

    PassphraseCache::cache_entry::cache_entry(const std::string email, const std::string& p) :
        email { email, 0, PassphraseCache::cache_entry::max_len },
        passphrase{ p, 0, PassphraseCache::cache_entry::max_len }
    {
    }

    PassphraseCache::PassphraseCache(size_t max_size, duration timeout) :
            _max_size{ max_size }, _timeout{ timeout }, _which(_cache.end()), first_time(true)
    {
    }

    PassphraseCache::PassphraseCache(const PassphraseCache& second) :
            _cache{ second._cache }, _max_size{ second._max_size }, _timeout{ second._timeout },
            _which(_cache.end()), first_time(true)
    {
        cleanup();
    }

    PassphraseCache& PassphraseCache::operator=(const PassphraseCache& second)
    {
        _cache = second._cache;
        _max_size = second._max_size;
        _timeout = second._timeout;
        _which = _cache.end();
        cleanup();
        return *this;
    }

    const PassphraseCache::cache_entry PassphraseCache::add(const cache_entry& entry)
    {
        const std::string& passphrase = entry.passphrase;
        const std::string& email = entry.email;
        if (!passphrase.empty() && !email.empty()) {
            std::lock_guard<std::mutex> lock(_mtx);

            _cache[email] = passphrase;
            // Check if the cache exceeds the maximum size and remove the oldest entry if necessary
            while (_cache.size() > _max_size) {
                auto it = _cache.begin();
                _cache.erase(it);
            }

            callback_dispatcher.semaphore.go();
            return {email, passphrase};
        }

        static const cache_entry empty("", "");
        return empty;
    }

//    const char* PassphraseCache::add_passphrase_for_new_keys(const std::string& passphrase) {
//        std::lock_guard<std::mutex> lock(_stored_mtx);
//        _new_keys_passphrase = cache_entry(PASSPHRASE_FOR_NEW_KEYS_ENTRY, passphrase);
//        return _new_keys_passphrase.passphrase.c_str();
//    }
//
//    const char* PassphraseCache::add_passphrase_for_new_keys(const cache_entry& entry) {
//        std::lock_guard<std::mutex> lock(_stored_mtx);
//        _new_keys_passphrase = entry;
//        return _new_keys_passphrase.passphrase.c_str();
//    }

    bool PassphraseCache::for_each_passphrase(const passphrase_callee& callee)
    {
        if (callee(cache_entry("", ""))) {
            return true;
        }

        {
            std::lock_guard<std::mutex> lock(_mtx);
            cleanup();

            for (auto it = _cache.begin(); it != _cache.end(); ++it) {
                const std::string& email = it->first;
                const std::string& passphrase = it->second;
                if (callee(cache_entry(email, passphrase))) {
                    //refresh(entry);
                    return true;
                }
            }
        }

        return false;
    }

    void PassphraseCache::cleanup()
    {
        //while (!_cache.empty() && _cache.front().tp < clock::now() - _timeout) {
        //    _cache.pop_front();
        //}
    }

    void PassphraseCache::refresh(cache::iterator entry)
    {
        //entry->tp = clock::now();
        //_cache.splice(_cache.end(), _cache, entry);
    }

    const PassphraseCache::cache_entry PassphraseCache::latest_passphrase(PassphraseCache& c)
    {
        if (c.first_time) {
            c.cleanup();
            c._which = c._cache.end();
            c.first_time = false;
        }

        if (c._cache.empty()) {
            c.first_time = true;
            //if (!c._new_keys_passphrase.email.empty() && !c._new_keys_passphrase.passphrase.empty()) {
            //    return c._new_keys_passphrase;
            //}
            throw Empty();
        }

        if (c._which == c._cache.begin()) {
            c.first_time = true;
            //if (!c._new_keys_passphrase.email.empty() && !c._new_keys_passphrase.passphrase.empty()) {
            //    return c._new_keys_passphrase;
            //}
            throw Exhausted();
        }

        --c._which;
        return cache_entry(c._which->first, c._which->second);
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
            cache_entry entry = latest_passphrase(_copy);
            ::config_passphrase(
                session != nullptr ? session : Adapter::session(),
                entry.email.c_str(), entry.passphrase.c_str());
            return PEP_STATUS_OK;
        } catch (pEp::PassphraseCache::Empty&) {
            new_copy = true;
            return PEP_PASSPHRASE_REQUIRED;
        } catch (pEp::PassphraseCache::Exhausted&) {
            new_copy = true;
            return PEP_WRONG_PASSPHRASE;
        }
    }

    PEP_STATUS PassphraseCache::ensure_passphrase(PEP_SESSION session, std::string email, std::string fpr)
    {
        PEP_STATUS status{ PEP_STATUS_OK };

        std::string passphrase = _cache[email];
        for_each_passphrase([&](const cache_entry& entry) {
            status = ::config_passphrase(session, entry.email.c_str(), entry.passphrase.c_str());
            if (status != 0) {
                return true;
            }
            status = ::probe_encrypt(session, fpr.c_str());
            return status == PEP_STATUS_OK;
        });

        return status;
    }
} // namespace pEp
