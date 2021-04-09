// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"

#include <iostream>
#include <vector>
#include <unistd.h>

#include <pEp/sync_api.h>

#include "../src/callback_dispatcher.hh"
#include "../src/passphrase_cache.hh"

using namespace std;
using namespace pEp;

vector<string> expected_msg = { "synchronizeGroupKeys",
                                "groupKeysUpdate",
                                "initUnledGroupKeyReset",
                                "beacon",
                                "beacon" };

vector<::sync_handshake_signal> expected_notification = { SYNC_NOTIFY_IN_GROUP,
                                                          SYNC_NOTIFY_START,
                                                          SYNC_NOTIFY_SOLE,
                                                          SYNC_NOTIFY_START,
                                                          SYNC_NOTIFY_STOP };

::PEP_STATUS test_messageToSend(::message* _msg)
{
    static auto actual = expected_msg.begin();

    Test::Message msg = Test::make_message(_msg);
    string text = Test::make_pEp_msg(msg);
    cerr << "expecting: " << *actual << endl;
    cerr << text;
    assert(text.find(*actual++) != string::npos);
    return PEP_STATUS_OK;
}


::PEP_STATUS test_notifyHandshake(::pEp_identity* _me, ::pEp_identity* _partner, sync_handshake_signal signal)
{
    static auto actual = expected_notification.begin();

    Test::Identity me = Test::make_identity(_me);
    Test::Identity partner = Test::make_identity(_partner);
    cerr << "expecting: " << *actual << endl;
    cerr << "notifyHandshake: " << signal << endl;
    assert(signal == *actual++);
    return PEP_STATUS_OK;
}

int main(int argc, char** argv)
{
    Test::setup(argc, argv);
    Adapter::session();

    // set up two own identites for sync

    passphrase_cache.add("erwin");
    passphrase_cache.add("bob");

    const char* bob_filename = ENGINE_TEST
        "/test_keys/bob-primary-with-password-bob-subkey-without.pgp";
    const char* bob_fpr = "5C76378A62B04CF3F41BEC8D4940FC9FA1878736";

    const char* erwin_filename = ENGINE_TEST "/test_keys/erwin_normal_encrypted.pgp";
    const char* erwin_fpr = "CBA968BC01FCEB89F04CCF155C5E9E3F0420A570";

    Test::import_key_from_file(bob_filename);
    Test::import_key_from_file(erwin_filename);

    Test::Identity bob = Test::make_identity(
        ::new_identity("bob@example.org", bob_fpr, "BOB", "Bob Dog"));
    PEP_STATUS status = ::set_own_key(Adapter::session(), bob.get(), bob_fpr);
    assert(status == PEP_STATUS_OK);

    status = ::enable_identity_for_sync(Adapter::session(), bob.get());
    assert(status == PEP_STATUS_OK);

    Test::Identity erwin = Test::make_identity(
        ::new_identity("erwin@example.org", erwin_fpr, "BOB", "Bob is Erwin"));
    status = ::set_own_key(Adapter::session(), erwin.get(), erwin_fpr);
    assert(status == PEP_STATUS_OK);

    status = ::enable_identity_for_sync(Adapter::session(), erwin.get());
    assert(status == PEP_STATUS_OK);

    // simulate a device group by setting the identities to in sync

    status = ::set_identity_flags(Adapter::session(), bob.get(), PEP_idf_devicegroup);
    status = ::set_identity_flags(Adapter::session(), erwin.get(), PEP_idf_devicegroup);

    // register at callback_dispatcher and start sync

    callback_dispatcher.add(test_messageToSend, test_notifyHandshake);
    CallbackDispatcher::start_sync();

    // leave device group


    status = ::leave_device_group(Adapter::session());

    // wait for sync shutdown and release first session

    Test::join_sync_thread();
    assert(!Adapter::is_sync_running());

    // switch off and on again

    CallbackDispatcher::start_sync();
    sleep(2);
    assert(Adapter::is_sync_running());
    CallbackDispatcher::stop_sync();
    Test::join_sync_thread();
    assert(!Adapter::is_sync_running());

    Adapter::session(Adapter::release);

    return 0;
}
