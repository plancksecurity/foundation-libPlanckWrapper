// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework.hh"
#include <iostream>
#include <cassert>
#include <cstring>
#include <sys/param.h>
#include <unistd.h>
#include <pEp/platform.h>
#include <message_cache.hh>
#include <Adapter.hh>

using namespace std;
using namespace pEp;

int main(int argc, char **argv)
{
    Test::setup(argc, argv);

    ::pEp_identity *alice = ::new_identity("alice@mail.com", nullptr, PEP_OWN_USERID, "Alice");
    ::myself(Adapter::session(), alice);

    char *mime = strdup("From: Alice <alice@mail.com>\n"
                        "To: Bob <bob@mail.com>\n"
                        "Subject: short message\n"
                        "\n"
                        "long message\n");

    // add to cache

    ::message *src = nullptr;
    bool has_possible_pEp_msg;
    ::PEP_STATUS status = MessageCache::cache_mime_decode_message(
        mime,
        strlen(mime),
        &src,
        &has_possible_pEp_msg);
    assert(status == PEP_STATUS_OK);

    status = ::myself(Adapter::session(), src->from);
    assert(status == PEP_STATUS_OK);

    ::update_identity(Adapter::session(), src->to->ident);
    assert(status == PEP_STATUS_OK);

    ::pEp_identity *bob = identity_dup(src->to->ident);

    src->dir = ::PEP_dir_outgoing;
    ::message *dst = nullptr;
    cout << "cache_encrypt_message()" << endl;
    status = MessageCache::cache_encrypt_message(
        Adapter::session(),
        src,
        nullptr,
        &dst,
        PEP_enc_PEP,
        0);
    assert(status != PEP_ILLEGAL_VALUE);

    assert(src->longmsg == nullptr);
    assert(src->attachments == nullptr);

    assert(dst == nullptr);

    // remove from cache

    free(mime);
    mime = nullptr;
    status = MessageCache::cache_mime_encode_message(MessageCache::msg_src, src, false, &mime, false);
    assert(status == PEP_STATUS_OK);
    cout << "cache_mime_encode_message()" << endl;
    cout << "mime: " << endl << mime << endl;

    // add to cache

    ::free_message(src);
    src = nullptr;
    cout << "cache_mime_decode_message" << endl;
    status = MessageCache::cache_mime_decode_message(mime, strlen(mime), &src, &has_possible_pEp_msg);
    assert(status == PEP_STATUS_OK);

    assert(src->longmsg == nullptr);
    assert(src->attachments == nullptr);

    ::PEP_rating rating;
    ::PEP_decrypt_flags_t flags = 0;
    ::stringlist_t *keylist = nullptr;

    cout << "cache_decrypt_message" << endl;
    status = MessageCache::cache_decrypt_message(
        Adapter::session(),
        src,
        &dst,
        &keylist,
        &rating,
        &flags);
    assert(status != PEP_ILLEGAL_VALUE);

    assert(src->longmsg == nullptr);
    assert(src->attachments == nullptr);

    // remove from cache

    free(mime);
    mime = nullptr;
    cout << "cache_mime_encode_message" << endl;
    status = MessageCache::cache_mime_encode_message(MessageCache::msg_src, src, false, &mime, false);

    assert(src->longmsg == nullptr);
    assert(src->attachments == nullptr);

    cout << "mime: " << endl << mime << endl;

    free(mime);
    ::free_message(src);
    ::free_message(dst);
    ::free_identity(bob);
    ::free_identity(alice);

    Adapter::session(Adapter::release);
    return 0;
}
