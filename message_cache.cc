#include "message_cache.hh"
#include <cassert>
#include <cstring>

pEp::MessageCache pEp::message_cache;

namespace pEp {
    PEP_STATUS MessageCache::cache_decrypt_message(
            PEP_SESSION session,
            message *src,
            message **dst,
            stringlist_t **keylist,
            PEP_rating *rating,
            PEP_decrypt_flags_t *flags
    )
    {
        return message_cache.decrypt_message(session, src, dst, keylist,
                rating, flags);
    }

    PEP_STATUS MessageCache::cache_mime_encode_message(
            int one,
            const message * msg,
            bool omit_fields,
            char **mimetext,
            bool has_pEp_msg_attachment     
        )
    {
        which _one = (which) one;
        return message_cache.mime_encode_message(_one, msg, omit_fields,
                mimetext, has_pEp_msg_attachment);
    }

    PEP_STATUS MessageCache::cache_mime_decode_message(
            const char *mimetext,
            size_t size,
            message **msg,
            bool* has_possible_pEp_msg
        )
    {
        return message_cache.mime_decode_message(mimetext, size, msg,
                has_possible_pEp_msg);
    }

    PEP_STATUS MessageCache::cache_encrypt_message(
            PEP_SESSION session,
            message *src,
            stringlist_t *extra,
            message **dst,
            PEP_enc_format enc_format,
            PEP_encrypt_flags_t flags
        )
    {
        return message_cache.encrypt_message(session, src, extra, dst,
                enc_format, flags);
    }

    PEP_STATUS MessageCache::cache_release(const char *id)
    {
        try {
            message_cache._cache.erase(std::string(id));
        }
        catch (...) { }
        return PEP_STATUS_OK;
    }

    static char *dup(const char *src)
    {
        if (!src)
            return nullptr;

        char * dst = ::strdup(src);
        assert(dst);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static timestamp *dup(const ::timestamp *src)
    {
        if (!src)
            return nullptr;

        ::timestamp * dst = ::timestamp_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static ::pEp_identity *dup(const ::pEp_identity *src)
    {
        if (!src)
            return nullptr;

        ::pEp_identity * dst = ::identity_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static identity_list *dup(const ::identity_list *src)
    {
        if (!src)
            return nullptr;

        ::identity_list * dst = ::identity_list_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static stringlist_t *dup(const ::stringlist_t *src)
    {
        if (!src)
            return nullptr;

        ::stringlist_t * dst = ::stringlist_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static stringpair_list_t *dup(const ::stringpair_list_t *src)
    {
        if (!src)
            return nullptr;

        ::stringpair_list_t * dst = ::stringpair_list_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static ::message_ref_list *dup(const ::message_ref_list *src) {
        if (!src)
            return nullptr;

        ::message_ref_list *dst = (::message_ref_list *) ::calloc(1,
                sizeof(::message_ref_list));
        assert(dst);
        if (!dst)
            throw std::bad_alloc();

        ::message_ref_list *d = dst;
        for (const message_ref_list *s = src; s; s = s->next) {
            d->msg_ref = s->msg_ref;
            if (s->next) {
                d->next = (::message_ref_list *) ::calloc(1,
                        sizeof(::message_ref_list));
                assert(d);
                if (!d)
                    throw std::bad_alloc();
                d = d->next;
            }
        }

        return dst;
    }

    static bool emptystr(const char *str)
    {
        if (!(str && str[0]))
            return true;
        return false;
    }

    ::message *MessageCache::empty_message_copy(::message *src)
    {
        if (!src)
            return nullptr;

        ::message *dst = ::new_message(src->dir);
        if (!dst)
            throw std::bad_alloc();

        dst->id = dup(src->id);

        if (!emptystr(src->shortmsg))
            dst->shortmsg = dup(src->shortmsg);
        else if (!emptystr(src->longmsg))
            dst->longmsg = dup("pEp");
        else if (!emptystr(src->longmsg_formatted))
            dst->longmsg_formatted = dup("<pEp/>");
        
        // attachments are never copied

        dst->rawmsg_ref = src->rawmsg_ref;
        dst->rawmsg_size = src->rawmsg_size;

        dst->sent = dup(src->sent);
        dst->recv = dup(src->recv);

        dst->from = dup(src->from);
        dst->to = dup(src->to);
        dst->cc = dup(src->cc);
        dst->bcc = dup(src->bcc);

        dst->reply_to = dup(src->reply_to);
        dst->in_reply_to = dup(src->in_reply_to);

        dst->refering_msg_ref = src->refering_msg_ref;
        dst->references = dup(src->references);
        dst->refered_by = dup(src->refered_by);

        dst->keywords = dup(src->keywords);
        dst->comments = dup(src->comments);
        dst->opt_fields = dup(src->opt_fields);
        dst->enc_format = src->enc_format;
        dst->_sender_fpr = dup(src->_sender_fpr);

        return dst;
    }

    DYNAMIC_API PEP_STATUS MessageCache::decrypt_message(
            PEP_SESSION session,
            message *src,
            message **dst,
            stringlist_t **keylist,
            PEP_rating *rating,
            PEP_decrypt_flags_t *flags
    )
    {
        if (!src || emptystr(src->id))
            return PEP_ILLEGAL_VALUE;

        PEP_STATUS status = ::decrypt_message(session, src, dst, keylist,
                rating, flags);

        ::message *_cpy = empty_message_copy(src);

        ::message *_src = (::message *) malloc(sizeof(::message));
        ::memcpy(_src, src, sizeof(::message));
        ::memcpy(src, _cpy, sizeof(::message));

        ::message *_dst = *dst;
        *dst = empty_message_copy(_dst);

        return status;
    }

    DYNAMIC_API PEP_STATUS MessageCache::mime_encode_message(
            which one,
            const message * msg,
            bool omit_fields,
            char **mimetext,
            bool has_pEp_msg_attachment     
        )
    {
        if (!msg || emptystr(msg->id))
            return PEP_ILLEGAL_VALUE;

        if (one != msg_src && one != msg_dst)
            return PEP_ILLEGAL_VALUE;

        ::message *_msg = ::message_dup(msg);
        if (one == msg_src) {
            ::message *_src = _cache.at(std::string(msg->id)).src;
            ::free_message(_cache.at(msg->id).dst);

            free(_msg->longmsg);
            _msg->longmsg = _src->longmsg;
            _src->longmsg = nullptr;

            free(_msg->longmsg_formatted);
            _msg->longmsg_formatted = _src->longmsg_formatted;
            _src->longmsg_formatted = nullptr;

            free_bloblist(_msg->attachments);
            _msg->attachments = _src->attachments;
            _src->attachments = nullptr;

            free_message(_src);
        }
        else /* msg_dst */ {
            ::message *_dst = _cache.at(std::string(msg->id)).dst;
            ::free_message(_cache.at(msg->id).src);

            free(_msg->longmsg);
            _msg->longmsg = _dst->longmsg;
            _dst->longmsg = nullptr;

            free(_msg->longmsg_formatted);
            _msg->longmsg_formatted = _dst->longmsg_formatted;
            _dst->longmsg_formatted = nullptr;

            free_bloblist(_msg->attachments);
            _msg->attachments = _dst->attachments;
            _dst->attachments = nullptr;

            free_message(_dst);
        }

        PEP_STATUS status = ::mime_encode_message(_msg, omit_fields, mimetext,
                has_pEp_msg_attachment);

        ::free_message(_msg);
        _cache.erase(msg->id);

        return status;
    }

    PEP_STATUS MessageCache::mime_decode_message(
            const char *mimetext,
            size_t size,
            message **msg,
            bool* has_possible_pEp_msg
        )
    {
        ::message *_msg = nullptr;
        PEP_STATUS status = ::mime_decode_message(mimetext, size, &_msg,
                has_possible_pEp_msg);
        if (status)
            return status;

        *msg = empty_message_copy(_msg);

        message_cache._cache.emplace(std::make_pair(std::string(_msg->id),
                    cache_entry(_msg, nullptr)));

        return status;
    }

    PEP_STATUS MessageCache::encrypt_message(
            PEP_SESSION session,
            message *src,
            stringlist_t *extra,
            message **dst,
            PEP_enc_format enc_format,
            PEP_encrypt_flags_t flags
        )
    {
        ::message *_msg = message_cache._cache.at(src->id).src;
        
        free(src->longmsg);
        src->longmsg = _msg->longmsg;
        _msg->longmsg = nullptr;

        free(src->longmsg_formatted);
        src->longmsg_formatted = _msg->longmsg_formatted;
        _msg->longmsg_formatted = nullptr;

        free_bloblist(src->attachments);
        src->attachments = _msg->attachments;
        _msg->attachments = nullptr;

        ::message *_dst = nullptr;
        PEP_STATUS status = ::encrypt_message(session, src, extra, &_dst,
                enc_format, flags);

        _msg->longmsg = src->longmsg;
        src->longmsg = nullptr;

        _msg->longmsg_formatted = src->longmsg_formatted;
        src->longmsg_formatted = nullptr;

        _msg->attachments = src->attachments;
        src->attachments = nullptr;
 
        *dst = empty_message_copy(_dst);
        ::free_message(message_cache._cache.at(src->id).dst);
        message_cache._cache.at(src->id).dst = _dst;

        return status;
    }
};

