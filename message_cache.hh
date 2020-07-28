#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <pEp/message_api.h>
#include <pEp/mime.h>

namespace pEp {
    class MessageCache {
        struct cache_entry {
            cache_entry(::message *s, ::message *d)
                : src(s), dst(d) { }

            ::message *src;
            ::message *dst;
        };

        using cache = std::unordered_map<std::string, cache_entry>;

        cache _cache;
        std::mutex _mtx;

    public:
        static 
        DYNAMIC_API PEP_STATUS cache_decrypt_message(
                PEP_SESSION session,
                message *src,
                message **dst,
                stringlist_t **keylist,
                PEP_rating *rating,
                PEP_decrypt_flags_t *flags
        );

        enum which { msg_src, msg_dst };;

        static
        DYNAMIC_API PEP_STATUS cached_mime_encode_message(
                which one,
                const message * msg,
                bool omit_fields,
                char **mimetext,
                bool has_pEp_msg_attachment     
            );

    protected:
        DYNAMIC_API PEP_STATUS decrypt_message(
                PEP_SESSION session,
                message *src,
                message **dst,
                stringlist_t **keylist,
                PEP_rating *rating,
                PEP_decrypt_flags_t *flags
        );

        DYNAMIC_API PEP_STATUS mime_encode_message(
                which one,
                const message * src,
                bool omit_fields,
                char **mimetext,
                bool has_pEp_msg_attachment
            );

        static ::message *empty_message_copy(::message *src);
    };

    extern MessageCache message_cache;
};

