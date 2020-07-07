#pragma once

#include <list>
#include <string>
#include <chrono>
#include <mutex>
#include <functional>
#include <exception>
#include <pEp/message_api.h>

#include "callback_dispatcher.hh"

namespace pEp {
    class PassphraseCache {
        using clock = std::chrono::system_clock;
        using time_point = std::chrono::time_point<clock>;
        using duration = clock::duration;

        struct cache_entry {
            static const size_t max_len = 250 * 4;
            cache_entry(const std::string&  p, time_point t);

            std::string passphrase;
            time_point tp;
        };
        using cache = std::list<cache_entry>;

        cache _cache;
        std::string _stored;
        std::mutex _mtx;
        std::mutex _stored_mtx;
        size_t _max_size;
        duration _timeout;

        cache::iterator _which;
        bool first_time;

        bool synchronous;

    public:
        struct Empty : public std::underflow_error {
            Empty() : std::underflow_error("passphrase cache empty") { }
        };
        struct Exhausted : public std::underflow_error {
            Exhausted() : std::underflow_error("out of passphrases") { }
        };

        PassphraseCache(size_t max_size=20, duration timeout = std::chrono::minutes(10));
        ~PassphraseCache() { }
        PassphraseCache(const PassphraseCache& second);
        PassphraseCache& operator=(const PassphraseCache& second);

        // switch this on for synchronous API
        void config_synchronous_api(bool enable = true) { synchronous = enable; }

        // adds the passphrase to the cache, which will timeout
        // returns a ptr to the passsword entry in the cache. Don't free() it!
        const char *add(const std::string& passphrase);

        // adds the stored passphrase to the cache, which will not timeout
        const char *add_stored(const std::string& passphrase);

        // call this function inside the messageToSend() implementation of the adapter
        // this function is using latest_passphrase() to test one passphrase after the
        // other until the cache is exhausted
        // call with reset = true to reset the iterator

        static PEP_STATUS config_next_passphrase(bool reset=false);

        // convenience functions
        // i.e.
        // status = cache.api(::encrypt_message, session, src, extra, dst, enc_format, flags)
        // will call
        // status = ::encrypt_message(session, src, extra, dst, enc_format, flags)
        // using for_each_passphrase()

        template<typename... A> PEP_STATUS api(PEP_STATUS f(PEP_SESSION, A...),
                PEP_SESSION session, A... a);

        static const char *latest_passphrase(PassphraseCache& _cache);
        using passphrase_callee = std::function<bool(std::string)>;
        bool for_each_passphrase(const passphrase_callee& callee);

    protected:
        void cleanup();
        void refresh(cache::iterator entry);
    };

    extern PassphraseCache passphrase_cache;
};

#include "passphrase_cache.hxx"

