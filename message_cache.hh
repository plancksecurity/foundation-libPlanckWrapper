// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_MESSAGE_CACHE_HH
#define LIBPEPADAPTER_MESSAGE_CACHE_HH

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
        long long id_range = 42;
        long long next_id = 23;

    public:
        MessageCache();

        enum which { msg_src = 0, msg_dst = 1 };

		static PEP_STATUS cache_mime_decode_message(
				const char *mimetext,
				size_t size,
				message **msg,
				bool* has_possible_pEp_msg
			);

        static PEP_STATUS cache_mime_encode_message(
                int one,
                const message * msg,
                bool omit_fields,
                char **mimetext,
                bool has_pEp_msg_attachment     
            );

        static PEP_STATUS cache_decrypt_message(
                PEP_SESSION session,
                message *src,
                message **dst,
                stringlist_t **keylist,
                PEP_rating *rating,
                PEP_decrypt_flags_t *flags
            );

		static PEP_STATUS cache_encrypt_message(
				PEP_SESSION session,
				message *src,
				stringlist_t *extra,
				message **dst,
				PEP_enc_format enc_format,
				PEP_encrypt_flags_t flags
			);

        static PEP_STATUS cache_encrypt_message_for_self(
                PEP_SESSION session,
                pEp_identity* target_id,
                message *src,
                stringlist_t* extra,
                message **dst,
                PEP_enc_format enc_format,
                PEP_encrypt_flags_t flags
            );


        static PEP_STATUS cache_release(std::string id);
        static void removeCacheID(::message* msg);

    protected:
        void release(std::string id);

		PEP_STATUS mime_decode_message(
				const char *mimetext,
				size_t size,
				message **msg,
				bool* has_possible_pEp_msg
			);

        PEP_STATUS mime_encode_message(
                which one,
                const message * src,
                bool omit_fields,
                char **mimetext,
                bool has_pEp_msg_attachment
            );

        PEP_STATUS decrypt_message(
                PEP_SESSION session,
                message *src,
                message **dst,
                stringlist_t **keylist,
                PEP_rating *rating,
                PEP_decrypt_flags_t *flags
            );

		PEP_STATUS encrypt_message(
				PEP_SESSION session,
				message *src,
				stringlist_t *extra,
				message **dst,
				PEP_enc_format enc_format,
				PEP_encrypt_flags_t flags
			);

        PEP_STATUS encrypt_message_for_self(
                PEP_SESSION session,
                pEp_identity* target_id,
                message *src,
                stringlist_t* extra,
                message **dst,
                PEP_enc_format enc_format,
                PEP_encrypt_flags_t flags
            );

        void generateCacheID(::message* msg);
        static std::string cacheID(const ::message* msg);
    };

    extern MessageCache message_cache;
};

#endif // LIBPEPADAPTER_MESSAGE_CACHE_HH
