// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"

#include <cassert>
#include <pEp/keymanagement.h>
#include <pEp/key_reset.h>

#include "../src/passphrase_cache.hh"
#include "../src/Adapter.hh"

using namespace pEp;
using namespace std;

int main(int argc, char** argv)
{
    pEp::Adapter::pEpLog::set_enabled(true);
    Test::setup(argc, argv);
    Adapter::session.initialize();
    passphrase_cache.add("erwin");
    passphrase_cache.add("cathy");
    passphrase_cache.add("bob");

    std::string bob_filename = Test::get_resource_abs("bob-primary-with-password-bob-subkey-without.pgp");
    std::string bob_fpr = "5C76378A62B04CF3F41BEC8D4940FC9FA1878736";

    std::string erwin_filename = Test::get_resource_abs("erwin_normal_encrypted.pgp");
    std::string erwin_fpr = "CBA968BC01FCEB89F04CCF155C5E9E3F0420A570";

    Test::import_key_from_file(bob_filename);
    Test::import_key_from_file(erwin_filename);

    ::pEp_identity* bob = ::new_identity("bob@example.org", bob_fpr.c_str(), "BOB", "Bob Dog");
    ::PEP_STATUS status = ::set_own_key(Adapter::session(), bob, bob_fpr.c_str());
    assert(status == PEP_STATUS_OK);

    ::pEp_identity* erwin = ::new_identity("erwin@example.org", erwin_fpr.c_str(), "BOB", "Bob is Erwin");
    status = ::set_own_key(Adapter::session(), erwin, erwin_fpr.c_str());
    assert(status == PEP_STATUS_OK);

    status = ::key_reset_all_own_keys(Adapter::session());
    assert(status == PEP_STATUS_OK);

    ::free_identity(bob);
    ::free_identity(erwin);

    // session(Adapter::release);

    return 0;
}
