#include <iostream>
#include <cassert>
#include "message_cache.hh"

using namespace std;
using namespace pEp;

int main()
{
    PEP_SESSION session;
    PEP_STATUS status = ::init(&session, nullptr, nullptr);
    assert(status == PEP_STATUS_OK);

    pEp_identity *alice = ::new_identity("alice@mail.com", nullptr, PEP_OWN_USERID, "Alice");
    ::myself(session, alice);

    pEp_identity *bob = ::new_identity("bob@mail.com", nullptr, PEP_OWN_USERID, "Bob");
    ::update_identity(session, bob);

    ::message *src = new_message(PEP_dir_incoming);
    src->from = identity_dup(alice);
    src->to = ::new_identity_list(::identity_dup(bob));

    src->shortmsg = strdup("short message");
    assert(src->shortmsg);

    src->longmsg = strdup("long message");
    assert(src->longmsg);

    src->longmsg_formatted = strdup("<long msg='formatted'/>");
    assert(src->longmsg_formatted);

    src->attachments = new_bloblist(strdup("blobdata"), 8, "application/octet-stream", "blob.data");
    assert(src->attachments && src->attachments->value);

    ::message *dst = nullptr;
    PEP_rating rating;
    PEP_decrypt_flags_t flags = 0;
    stringlist_t *keylist = nullptr;

    status = MessageCache::cache_decrypt_message(session, src, &dst, &keylist, &rating, &flags);
    assert(status == PEP_ILLEGAL_VALUE);

    src->id = strdup("42");
    assert(src->id);
    status = MessageCache::cache_decrypt_message(session, src, &dst, &keylist, &rating, &flags);
    assert(status != PEP_ILLEGAL_VALUE);

    assert(string(src->longmsg) == "pEp");
    assert(src->attachments == nullptr);

    cout << status << endl;

    ::free_message(src);
    ::free_message(dst);
    ::free_identity(bob);
    ::free_identity(alice);
    ::release(session);
    return 0;
}

