// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "message_cache.hh"
#include <pEp/platform.h>
#include <cassert>
#include <cstring>
#include <climits>
#include <random>

pEp::MessageCache pEp::message_cache;

namespace pEp {
    MessageCache::MessageCache()
    {
        std::random_device r;
        std::default_random_engine e(r());
        std::uniform_int_distribution<long long> u(1, LLONG_MAX >> 1);
        id_range = u(e);
        next_id = u(e);
    }

    PEP_STATUS MessageCache::cache_decrypt_message(
        PEP_SESSION session,
        message *src,
        message **dst,
        stringlist_t **keylist,
        PEP_rating *rating,
        PEP_decrypt_flags_t *flags)
    {
        return message_cache.decrypt_message(session, src, dst, keylist, rating, flags);
    }

    PEP_STATUS MessageCache::cache_mime_encode_message(
        int one,
        const message *msg,
        bool omit_fields,
        char **mimetext,
        bool has_pEp_msg_attachment)
    {
        which _one = (which)one;
        return message_cache.mime_encode_message(_one, msg, omit_fields, mimetext, has_pEp_msg_attachment);
    }

    PEP_STATUS MessageCache::cache_mime_decode_message(
        const char *mimetext,
        size_t size,
        message **msg,
        bool *has_possible_pEp_msg)
    {
        return message_cache.mime_decode_message(mimetext, size, msg, has_possible_pEp_msg);
    }

    PEP_STATUS MessageCache::cache_encrypt_message(
        PEP_SESSION session,
        message *src,
        stringlist_t *extra,
        message **dst,
        PEP_enc_format enc_format,
        PEP_encrypt_flags_t flags)
    {
        return message_cache.encrypt_message(session, src, extra, dst, enc_format, flags);
    }

    PEP_STATUS MessageCache::cache_encrypt_message_for_self(
        PEP_SESSION session,
        pEp_identity *target_id,
        message *src,
        stringlist_t *extra,
        message **dst,
        PEP_enc_format enc_format,
        PEP_encrypt_flags_t flags)
    {
        return message_cache
            .encrypt_message_for_self(session, target_id, src, extra, dst, enc_format, flags);
    }

    PEP_STATUS MessageCache::cache_release(std::string id)
    {
        message_cache.release(id);
        return PEP_STATUS_OK;
    }

    void MessageCache::release(std::string id)
    {
        try {
            std::lock_guard<std::mutex> l(message_cache._mtx);
            ::free_message(_cache.at(id).src);
            ::free_message(_cache.at(id).dst);
            _cache.erase(id);
        } catch (...) {
        }
    }

    static char *dup(const char *src)
    {
        if (!src)
            return nullptr;

        char *dst = ::strdup(src);
        assert(dst);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static timestamp *dup(const ::timestamp *src)
    {
        if (!src)
            return nullptr;

        ::timestamp *dst = ::timestamp_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static ::pEp_identity *dup(const ::pEp_identity *src)
    {
        if (!src)
            return nullptr;

        ::pEp_identity *dst = ::identity_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static identity_list *dup(const ::identity_list *src)
    {
        if (!src)
            return nullptr;

        ::identity_list *dst = ::identity_list_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static stringlist_t *dup(const ::stringlist_t *src)
    {
        if (!src)
            return nullptr;

        ::stringlist_t *dst = ::stringlist_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static stringpair_list_t *dup(const ::stringpair_list_t *src)
    {
        if (!src)
            return nullptr;

        ::stringpair_list_t *dst = ::stringpair_list_dup(src);
        if (!dst)
            throw std::bad_alloc();

        return dst;
    }

    static ::message_ref_list *dup(const ::message_ref_list *src)
    {
        if (!src)
            return nullptr;

        ::message_ref_list *dst = (::message_ref_list *)::calloc(1, sizeof(::message_ref_list));
        assert(dst);
        if (!dst)
            throw std::bad_alloc();

        ::message_ref_list *d = dst;
        for (const message_ref_list *s = src; s; s = s->next) {
            d->msg_ref = s->msg_ref;
            if (s->next) {
                d->next = (::message_ref_list *)::calloc(1, sizeof(::message_ref_list));
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

    static ::message *empty_message_copy(const ::message *src, std::string _id = "", bool get_longmsg = false)
    {
        if (!src)
            return nullptr;

        ::message *dst = ::new_message(src->dir);
        if (!dst)
            throw std::bad_alloc();

        dst->id = dup(src->id);

        if (get_longmsg) {
            if (!emptystr(src->shortmsg)) {
                dst->shortmsg = dup(src->shortmsg);
            }
            // We need either longmsg or longmsg_formatted for a "message preview".
            if (!emptystr(src->longmsg)) {
                dst->longmsg = dup(src->longmsg);
            } else {
                if (!emptystr(src->longmsg_formatted)) {
                    dst->longmsg_formatted = dup(src->longmsg_formatted);
                }
            }
        } else {
            // It is a pEp convention to return at least one of shortmsg, longmsg or longmsg_formatted.
            if (!emptystr(src->shortmsg)) {
                dst->shortmsg = dup(src->shortmsg);
            } else {
                if (!emptystr(src->longmsg)) {
                    dst->longmsg = dup("pEp");
                } else {
                    if (!emptystr(src->longmsg_formatted)) {
                        dst->longmsg_formatted = dup("<pEp/>");
                    }
                }
            }
        }

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
        dst->enc_format = src->enc_format;
        dst->_sender_fpr = dup(src->_sender_fpr);

        if (_id == "") {
            dst->opt_fields = dup(src->opt_fields);
        } else {
            dst->opt_fields = ::new_stringpair_list(
                ::new_stringpair("X-pEp-Adapter-Cache-ID", _id.c_str()));
            if (!dst->opt_fields)
                throw std::bad_alloc();
            dst->opt_fields->next = dup(src->opt_fields);
        }

        dst->rating = src->rating;

        return dst;
    }

    static void correctAttachmentsOrder(bloblist_t *&bl)
    {
        // only execute if there are exactly two attachments, both with
        // a non-empty MIME type

        if (bl && bl->next && !bl->next->next && !emptystr(bl->mime_type) &&
            !emptystr(bl->next->mime_type)) {

            // if this is most likely an PGP/MIME compliant format then correct
            // order of attachments

            if (std::string(bl->mime_type) == "application/octet-stream" &&
                std::string(bl->next->mime_type) == "application/pgp-encrypted") {
                bloblist_t *one = bl->next;
                bloblist_t *two = bl;
                bl = one;
                bl->next = two;
                bl->next->next = nullptr;
            }
        }
    }

    static void swapContent(::message *&part, ::message *&full)
    {
        free(part->longmsg);
        part->longmsg = full->longmsg;
        full->longmsg = nullptr;

        free(part->longmsg_formatted);
        part->longmsg_formatted = full->longmsg_formatted;
        full->longmsg_formatted = nullptr;

        free_bloblist(part->attachments);
        part->attachments = full->attachments;
        full->attachments = nullptr;
    }

    PEP_STATUS MessageCache::decrypt_message(
        PEP_SESSION session,
        message *src,
        message **dst,
        stringlist_t **keylist,
        PEP_rating *rating,
        PEP_decrypt_flags_t *flags)
    {
        if (!src || cacheID(src) == "")
            return PEP_ILLEGAL_VALUE;

        ::message *_msg;
        std::string _id = cacheID(src);
        {
            std::lock_guard<std::mutex> l(_mtx);
            _msg = message_cache._cache.at(_id).src;
            swapContent(src, _msg);
        }

        // if attachments got reordered correct
        correctAttachmentsOrder(src->attachments);

        ::message *_dst = nullptr;
        PEP_STATUS status = ::decrypt_message(session, src, &_dst, keylist, rating, flags);
        *dst = empty_message_copy(_dst, _id, true);

        {
            std::lock_guard<std::mutex> l(_mtx);
            swapContent(_msg, src);
            ::free_message(message_cache._cache.at(_id).dst);
            message_cache._cache.at(_id).dst = _dst;
        }
        return status;
    }

    PEP_STATUS MessageCache::mime_encode_message(
        which one,
        const message *msg,
        bool omit_fields,
        char **mimetext,
        bool has_pEp_msg_attachment)
    {
        if (!msg || cacheID(msg) == "")
            return PEP_ILLEGAL_VALUE;

        if (one != msg_src && one != msg_dst)
            return PEP_ILLEGAL_VALUE;

        ::message *_msg = empty_message_copy(msg);

        if (one == msg_src) {
            std::lock_guard<std::mutex> l(_mtx);
            ::message *_src = _cache.at(cacheID(msg)).src;
            swapContent(_msg, _src);
        } else /* msg_dst */ {
            std::lock_guard<std::mutex> l(_mtx);
            ::message *_dst = _cache.at(cacheID(msg)).dst;
            swapContent(_msg, _dst);
        }

        removeCacheID(_msg);
        PEP_STATUS status = ::mime_encode_message(_msg, omit_fields, mimetext, has_pEp_msg_attachment);
        ::free_message(_msg);

        cache_release(cacheID(msg));

        return status;
    }

    void MessageCache::generateCacheID(::message *msg)
    {
        std::string _range = std::to_string(id_range);
        std::string _id = std::to_string(next_id++);
        std::string cid = _range + _id;

        // if opt_fields is an empty list generate a new list
        if (!msg->opt_fields || !msg->opt_fields->value) {
            free_stringpair_list(msg->opt_fields);
            msg->opt_fields = ::new_stringpair_list(
                ::new_stringpair("X-pEp-Adapter-Cache-ID", cid.c_str()));
            if (!msg->opt_fields)
                throw std::bad_alloc();
        } else {
            // add the cache ID as first field to an existing list
            auto spl = msg->opt_fields;
            msg->opt_fields = ::new_stringpair_list(
                ::new_stringpair("X-pEp-Adapter-Cache-ID", cid.c_str()));
            if (!msg->opt_fields) {
                msg->opt_fields = spl;
                throw std::bad_alloc();
            }
            msg->opt_fields->next = spl;
        }
    }

    std::string MessageCache::cacheID(const ::message *msg)
    {
        for (auto spl = msg->opt_fields; spl && spl->value; spl = spl->next) {
            assert(spl->value->key);
            if (spl->value->key && std::string(spl->value->key) == "X-pEp-Adapter-Cache-ID") {
                assert(spl->value->value);
                if (spl->value->value)
                    return spl->value->value;
                else
                    return "";
            }
        }
        return "";
    }

    void MessageCache::removeCacheID(::message *msg)
    {
        // if the first element in the list is the cache ID then skip
        if (msg->opt_fields && msg->opt_fields->value && msg->opt_fields->value->key &&
            std::string(msg->opt_fields->value->key) == "X-pEp-Adapter-Cache-ID") {
            auto n = msg->opt_fields->next;
            msg->opt_fields->next = nullptr;
            ::free_stringpair_list(msg->opt_fields);
            msg->opt_fields = n;
        } else {
            // go through the list and remove
            ::stringpair_list_t *prev = nullptr;
            for (auto spl = msg->opt_fields; spl && spl->value; spl = spl->next) {
                assert(spl->value->key);
                if (spl->value->key && std::string(spl->value->key) == "X-pEp-Adapter-Cache-ID") {
                    auto next = spl->next;
                    spl->next = nullptr;
                    ::free_stringpair_list(spl);
                    prev->next = next;
                    break;
                }
                prev = spl;
            }
        }
    }

    PEP_STATUS MessageCache::mime_decode_message(
        const char *mimetext,
        size_t size,
        message **msg,
        bool *has_possible_pEp_msg)
    {
        ::message *_msg = nullptr;
        PEP_STATUS status = ::mime_decode_message(mimetext, size, &_msg, has_possible_pEp_msg);
        if (status)
            return status;

        generateCacheID(_msg);
        *msg = empty_message_copy(_msg);

        {
            std::lock_guard<std::mutex> l(_mtx);
            message_cache._cache.emplace(std::make_pair(cacheID(_msg), cache_entry(_msg, nullptr)));
        }

        return status;
    }

    PEP_STATUS MessageCache::encrypt_message(
        PEP_SESSION session,
        message *src,
        stringlist_t *extra,
        message **dst,
        PEP_enc_format enc_format,
        PEP_encrypt_flags_t flags)
    {
        ::message *_msg;
        std::string _id = cacheID(src);
        {
            std::lock_guard<std::mutex> l(_mtx);
            _msg = message_cache._cache.at(_id).src;
            swapContent(src, _msg);
        }

        ::message *_dst = nullptr;
        PEP_STATUS status = ::encrypt_message(session, src, extra, &_dst, enc_format, flags);
        *dst = empty_message_copy(_dst, _id);

        {
            std::lock_guard<std::mutex> l(_mtx);
            swapContent(_msg, src);
            ::free_message(message_cache._cache.at(_id).dst);
            message_cache._cache.at(_id).dst = _dst;
        }

        return status;
    }

    PEP_STATUS MessageCache::encrypt_message_for_self(
        PEP_SESSION session,
        pEp_identity *target_id,
        message *src,
        stringlist_t *extra,
        message **dst,
        PEP_enc_format enc_format,
        PEP_encrypt_flags_t flags)
    {
        ::message *_msg;
        std::string _id = cacheID(src);
        {
            std::lock_guard<std::mutex> l(_mtx);
            _msg = message_cache._cache.at(_id).src;
            swapContent(src, _msg);
        }

        ::message *_dst = nullptr;
        PEP_STATUS status = ::encrypt_message_for_self(
            session,
            target_id,
            src,
            extra,
            &_dst,
            enc_format,
            flags);
        *dst = empty_message_copy(_dst, _id);

        {
            std::lock_guard<std::mutex> l(_mtx);
            swapContent(_msg, src);
            ::free_message(message_cache._cache.at(_id).dst);
            message_cache._cache.at(_id).dst = _dst;
        }

        return status;
    }
}; // namespace pEp
