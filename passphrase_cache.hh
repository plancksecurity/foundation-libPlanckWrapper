#pragma once

#include <list>
#include <string>
#include <chrono>
#include <mutex>
#include <functional>
#include <pEp/message_api.h>

namespace pEp {
    class PassphraseCache {
        using clock = std::chrono::system_clock;
        using time_point = std::chrono::time_point<clock>;
        using duration = clock::duration;

        struct cache_entry {
            static const size_t max_len = 250 * 4;
            cache_entry(std::string p, time_point t) :
                passphrase(p, 0, max_len), tp(t) { }

            std::string passphrase;
            time_point tp;
        };
        using cache = std::list<cache_entry>;
        using interator = cache::iterator;
        using const_iterator = cache::const_iterator;

        cache _cache;   
        std::mutex _mtx;
        int _max_size;
        duration _timeout;

    public:
        PassphraseCache(int max_size=20, duration timeout =
                std::chrono::minutes(10)) : _max_size(max_size),
                    _timeout(timeout) { }
        ~PassphraseCache() { }
        PassphraseCache(const PassphraseCache& second) : _cache(second._cache),
                _max_size(second._max_size), _timeout(second._timeout) { }
        PassphraseCache operator=(const PassphraseCache& second) { return second; }

        // adding a passphrase to the cache, which will timeout

        const char *add(std::string passphrase);

        // for each passphrase call the callee until it returns true for a
        // matching passphrase or no passphrases are left
        // always tests empty passphrase first
        // returns true if a passphrase was matching, false otherwise

        using passphrase_callee = std::function<bool(std::string)>;
        bool for_each_passphrase(const passphrase_callee& callee);

        // convenience functions
        // i.e.
        // status = cache.api(::encrypt_message, session, src, extra, dst, enc_format, flags)
        // will call
        // status = ::encrypt_message(session, src, extra, dst, enc_format, flags)
        // using for_each_passphrase()

        template<typename... A> PEP_STATUS api(PEP_STATUS f(PEP_SESSION, A...),
                PEP_SESSION session, A... a);

    protected:
        void cleanup();
        void refresh(cache::iterator entry);
    };
};

#include "passphrase_cache.hxx"

