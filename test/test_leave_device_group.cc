#include <iostream>

#include "framework.hh"
#include "passphrase_cache.hh"
#include "callback_dispatcher.hh"

#include <pEp/sync_api.h>

using namespace pEp;
using namespace pEp::Adapter;
using namespace std;

PEP_STATUS test_messageToSend(::message *_msg)
{
    Test::Message msg = Test::make_message(_msg);
    cerr << Test::make_pEp_msg(msg);
    return PEP_STATUS_OK;   
}


PEP_STATUS test_notifyHandshake(pEp_identity *_me, pEp_identity *_partner, sync_handshake_signal signal)
{
    Test::Identity me = Test::make_identity(_me);
    Test::Identity partner = Test::make_identity(_partner);

    return PEP_STATUS_OK;   
}

int main(int argc, char **argv)
{
    Test::setup(argc, argv);

    // set up two own identites for sync

    passphrase_cache.add("erwin");
    passphrase_cache.add("bob");

    const char* bob_filename = ENGINE_TEST "/test_keys/bob-primary-with-password-bob-subkey-without.pgp";
    const char* bob_fpr = "5C76378A62B04CF3F41BEC8D4940FC9FA1878736";

    const char* erwin_filename = ENGINE_TEST "/test_keys/erwin_normal_encrypted.pgp";
    const char* erwin_fpr = "CBA968BC01FCEB89F04CCF155C5E9E3F0420A570";

    Test::import_key_from_file(bob_filename);
    Test::import_key_from_file(erwin_filename);

    pEp_identity* bob = ::new_identity("bob@example.org", bob_fpr, "BOB", "Bob Dog");
    PEP_STATUS status = ::set_own_key(session(), bob, bob_fpr);
    assert(status == PEP_STATUS_OK);

    status = ::enable_identity_for_sync(session(), bob);
    assert(status == PEP_STATUS_OK);

    pEp_identity* erwin = ::new_identity("erwin@example.org", erwin_fpr, "BOB", "Bob is Erwin");
    status = ::set_own_key(session(), erwin, erwin_fpr);
    assert(status == PEP_STATUS_OK);

    status = ::enable_identity_for_sync(session(), erwin);
    assert(status == PEP_STATUS_OK);

    // simulate a device group by setting the identities to in sync

    status = set_identity_flags(session(), bob, PEP_idf_devicegroup);
    status = set_identity_flags(session(), erwin, PEP_idf_devicegroup);

    // register at callback_dispatcher and start sync

    callback_dispatcher.add(test_messageToSend, test_notifyHandshake);
    CallbackDispatcher::start_sync();


    // stop sync
    CallbackDispatcher::stop_sync();

    // free own identities and release session and release session

    ::free_identity(bob);
    ::free_identity(erwin);

    session(Adapter::release);

    return 0;
}

