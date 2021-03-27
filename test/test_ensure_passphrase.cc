#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "framework.hh"

#include "passphrase_cache.hh"
#include "status_to_string.hh"

#include <pEp/message_api.h>
#include <pEp/keymanagement.h>
#include <pEp/key_reset.h>

using namespace pEp;
using namespace pEp::Adapter;
using namespace std;

int main(int argc, char** argv)
{
    Test::setup(argc, argv);

    passphrase_cache.add("erwin");
    passphrase_cache.add("cathy");
    passphrase_cache.add("bob");

    const char* bob_filename = ENGINE_TEST
        "/test_keys/bob-primary-with-password-bob-subkey-without.pgp";
    const char* bob_fpr = "5C76378A62B04CF3F41BEC8D4940FC9FA1878736";

    const char* erwin_filename = ENGINE_TEST "/test_keys/erwin_normal_encrypted.pgp";
    const char* erwin_fpr = "CBA968BC01FCEB89F04CCF155C5E9E3F0420A570";

    Test::import_key_from_file(bob_filename);
    Test::import_key_from_file(erwin_filename);

    pEp_identity* bob = ::new_identity("bob@example.org", bob_fpr, "BOB", "Bob Dog");
    PEP_STATUS status = ::set_own_key(session(), bob, bob_fpr);
    assert(status == PEP_STATUS_OK);

    pEp_identity* erwin = ::new_identity("erwin@example.org", erwin_fpr, "BOB", "Bob is Erwin");
    status = ::set_own_key(session(), erwin, erwin_fpr);
    assert(status == PEP_STATUS_OK);

    status = ::key_reset_all_own_keys(session());
    assert(status == PEP_STATUS_OK);

    ::free_identity(bob);
    ::free_identity(erwin);

    // session(Adapter::release);

    return 0;
}
