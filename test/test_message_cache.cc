#include <iostream>
#include <cassert>
#include <sys/param.h>
#include <unistd.h>
#include "message_cache.hh"
#include "Adapter.hh"

using namespace std;
using namespace pEp;

int main()
{
    char path[MAXPATHLEN+1];
    const char *templ = "/tmp/test_message_cache.XXXXXXXXXXXX";
    strcpy(path, templ);
    char *tmpdir = mkdtemp(path);
    assert(tmpdir);
    chdir(tmpdir);
    setenv("HOME", path, 1);
    cerr << "test directory: " << path << endl;

    pEp_identity *alice = ::new_identity("alice@mail.com", nullptr, PEP_OWN_USERID, "Alice");
    ::myself(pEp::Adapter::session(), alice);

    char *mime = strdup("From: Alice <alice@mail.com>\n"
                        "To: Bob <bob@mail.com>\n"
                        "Subject: short message\n"
                        "\n"
                        "long message\n");

    // add to cache

    ::message *src = nullptr;
    bool has_possible_pEp_msg;
    PEP_STATUS status = MessageCache::cache_mime_decode_message(mime, strlen(mime), &src, &has_possible_pEp_msg);
    assert(status == PEP_STATUS_OK);

    status = ::myself(pEp::Adapter::session(), src->from);
    assert(status == PEP_STATUS_OK);

    ::update_identity(pEp::Adapter::session(), src->to->ident);
    assert(status == PEP_STATUS_OK);

    pEp_identity *bob = identity_dup(src->to->ident);

    src->dir = PEP_dir_outgoing;
    ::message *dst = nullptr;
    status = MessageCache::cache_encrypt_message(pEp::Adapter::session(), src, nullptr, &dst, PEP_enc_PEP, 0);
    assert(status != PEP_ILLEGAL_VALUE);

    assert(src->longmsg == nullptr);
    assert(src->attachments == nullptr);

    assert(dst == nullptr);

    // remove from cache

    free(mime);
    mime = nullptr;
    status = MessageCache::cache_mime_encode_message(MessageCache::msg_src, src, false, &mime, false);
    assert(status == PEP_STATUS_OK);

    cout << mime << endl;

    // add to cache

    ::free_message(src);
    src = nullptr;
    status = MessageCache::cache_mime_decode_message(mime, strlen(mime), &src, &has_possible_pEp_msg);
    assert(status == PEP_STATUS_OK);

    assert(src->longmsg == nullptr);
    assert(src->attachments == nullptr);

    PEP_rating rating;
    PEP_decrypt_flags_t flags = 0;
    stringlist_t *keylist = nullptr;

    status = MessageCache::cache_decrypt_message(pEp::Adapter::session(), src, &dst, &keylist, &rating, &flags);
    assert(status != PEP_ILLEGAL_VALUE);

    assert(src->longmsg == nullptr);
    assert(src->attachments == nullptr);

    // remove from cache

    free(mime);
    mime = nullptr;
    status = MessageCache::cache_mime_encode_message(MessageCache::msg_src, src, false, &mime, false);

    assert(src->longmsg == nullptr);
    assert(src->attachments == nullptr);

    cout << mime << endl;

    free(mime);
    ::free_message(src);
    ::free_message(dst);
    ::free_identity(bob);
    ::free_identity(alice);

    pEp::Adapter::session(pEp::Adapter::release);
    return 0;
}

