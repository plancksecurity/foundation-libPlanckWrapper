// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PASSPHRASE_CACHE_HH
#define LIBPEPADAPTER_PASSPHRASE_CACHE_HH

#include <list>
#include <string>
#include <chrono>
#include <mutex>
#include <functional>
#include <exception>
#include <pEp/message_api.h>

namespace pEp {
    class PassphraseCache {
        using clock = std::chrono::system_clock;
        using time_point = std::chrono::time_point<clock>;
        using duration = clock::duration;

        struct cache_entry {
            static const size_t max_len = static_cast<const size_t>(250 * 4);
            cache_entry(const std::string& p, time_point t);
            cache_entry(const std::string& e, const std::string& p, time_point t);

            std::string account_email;
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

    public:
        struct Empty : public std::underflow_error {
            Empty() : std::underflow_error("passphrase cache empty") {}
        };
        struct Exhausted : public std::underflow_error {
            Exhausted() : std::underflow_error("out of passphrases") {}
        };

        PassphraseCache(size_t max_size = 20, duration timeout = std::chrono::minutes(180));
        ~PassphraseCache() = default;
        PassphraseCache(const PassphraseCache& second);
        PassphraseCache& operator=(const PassphraseCache& second);

        // adds a passphrase to the cache
        // returns a ptr to the passsword entry in the cache. Don't free() it!
        const char* add(const std::string& passphrase);

        /// <summary>
        /// Adds an (account_email, passphrase) tuple to the cache.
        /// </summary>
        ///
        /// The `account_email` is treated as unique, that is, there cannot be
        /// several entries with the same `account_email`.
        /// A later call with the same `account_email` will overwrite the existing one.
        ///
        /// <param name="account_email"></param>
        /// <param name="passphrase"></param>
        /// <returns>A ptr to the passsword entry in the cache. Don't free() it!</returns>
        const char* add(const std::string& account_email, const std::string& passphrase);

        /// <summary>
        /// Removes all entries that share the same account_email.
        /// </summary>
        /// <param name="account_email">The account email to remove</param>
        void remove(const std::string& account_email);

        // adds the stored passphrase to the cache, which will not timeout
        const char* add_stored(const std::string& passphrase);

        // call this function inside the messageToSend() implementation of the adapter
        // this function is using latest_passphrase() to test one passphrase after the
        // other until the cache is exhausted
        // call with reset = true to reset the iterator

        static PEP_STATUS config_next_passphrase(bool reset = false, PEP_SESSION session = nullptr);

        // convenience functions
        // i.e.
        // status = cache.api(::encrypt_message, session, src, extra, dst, enc_format, flags)
        // will call
        // status = ::encrypt_message(session, src, extra, dst, enc_format, flags)
        // using for_each_passphrase()

        template<typename... A>
        PEP_STATUS api(PEP_STATUS f(PEP_SESSION, A...), PEP_SESSION session, A... a);

        static const char* latest_passphrase(PassphraseCache& _cache);
        using passphrase_callee = std::function<bool(std::string)>;
        bool for_each_passphrase(const passphrase_callee& callee);
        PEP_STATUS ensure_passphrase(PEP_SESSION session, std::string fpr);
        void configure_session(PEP_SESSION session);

    protected:
        void cleanup();
        void refresh(cache::iterator entry);
        const char *add(const cache_entry& entry);
    };

    extern PassphraseCache passphrase_cache;
} // namespace pEp

#include "passphrase_cache.hxx"

#endif // LIBPEPADAPTER_PASSPHRASE_CACHE_HH
