#pragma once

#include <list>
#include <string>
#include <chrono>
#include <mutex>
#include <functional>

namespace pEp {
    class PassphraseCache {
        using clock = std::chrono::system_clock;
        using time_point = std::chrono::time_point<clock>;
        using duration = clock::duration;

        struct cache_entry {
            std::string passphrase;
            time_point tp;
        };

        std::list<cache_entry> _cache;   
        std::mutex _mtx;
        duration _timeout;

    public:
        PassphraseCache(duration timeout = std::chrono::minutes(10)) :
            _timeout(timeout) { }
        ~PassphraseCache() { }
        PassphraseCache(const PassphraseCache& second) :
            _cache(second._cache), _timeout(second._timeout) { }

        // adding a passphrase to the cache, which will timeout
        void add(std::string passphrase);

        // for each passphrase call the callee until it returns true for a
        // matching passphrase or no passphrases are left
        // returns true if a passphrase was matching, false otherwise
        using passphrase_callee = std::function<bool(std::string)>;
        bool for_each_passphrase(const passphrase_callee& callee);

    protected:
        void cleanup();
    };
};

