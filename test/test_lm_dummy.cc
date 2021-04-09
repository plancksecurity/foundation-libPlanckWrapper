// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"
#include "framework/utils.hh"

#include <iostream>

#include "../src/Adapter.hh"
#include "../src/status_to_string.hh"
#include "../src/ListManagerDummy.hh"

using namespace std;
using namespace pEp;
using namespace pEp::Test::Log;

bool debug_info_full = true;
::pEp_identity* alice = nullptr;
::pEp_identity* bob = nullptr;
::pEp_identity* carol = nullptr;
::pEp_identity* grp_ident = nullptr;
::PEP_STATUS status;

ListManagerInterface* lm_backend;

/*
 * Test Units
 */

void test_create_alice_me()
{
    logH2("test_create_alice_me");
    alice = ::new_identity("alice@peptest.ch", NULL, "23", "Alice");
    assert(alice);
    alice->lang[0] = 'e';
    alice->lang[1] = 'n';
    status = ::myself(Adapter::session(), alice);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "Alice:" << Test::Utils::identity_to_string(alice, debug_info_full) << endl;
}

void test_create_bob_partner()
{
    logH2("test_create_bob_partner");
    bob = ::new_identity("bob@peptest.ch", NULL, PEP_OWN_USERID, "Bob");
    assert(bob);
    bob->lang[0] = 'c';
    bob->lang[1] = 'r';
    status = ::update_identity(Adapter::session(), bob);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "Bob:" << Test::Utils::identity_to_string(bob, debug_info_full) << endl;
}

void test_create_carol_partner()
{
    logH2("test_create_carol_partner");
    carol = ::new_identity("carol@peptest.ch", NULL, PEP_OWN_USERID, "Carol");
    assert(carol);
    carol->lang[0] = 'f';
    carol->lang[1] = 'n';
    status = ::update_identity(Adapter::session(), carol);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "Carol:" << Test::Utils::identity_to_string(carol, debug_info_full) << endl;
}


void test_group_create(::identity_list* idl)
{
    logH2("test_group_create");
    cout << "IDL: " << Test::Utils::identitylist_to_string(idl, debug_info_full) << endl;

    cout << "create group identity" << endl;
    grp_ident = ::new_identity("group1@peptest.ch", NULL, "432", "group1");
    assert(grp_ident);
    status = ::myself(Adapter::session(), grp_ident);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "grp_ident:" << Test::Utils::identity_to_string(grp_ident, debug_info_full) << endl;

    lm_backend->create(grp_ident, alice, idl);
}

void test_group_join(::pEp_identity* ident)
{
    logH2("test_group_join");
    lm_backend->join(grp_ident, ident);
}

void test_group_remove_member(::pEp_identity* ident)
{
    logH2("test_group_remove_member");
    lm_backend->remove_member(grp_ident, ident);
}

void test_group_dissolve()
{
    logH2("test_group_dissolve");
    lm_backend->dissolve(grp_ident);
}


/*
 * Update functions
 * ----------------
 * Test procedure:
 * 1. Create group
 * - group_create(Alice)
 * 2. Add Bob
 * - group_join(Bob)
 * 3. Add Carol
 * - group_join(Carol)
 * 4. Remove Carol
 * - group_remove_member(Carol)
 * 6. Dissolve
 * - group_dissolve()
 *
 * Query functions
 * ---------------
 * Always test all the query functions for correctness between every step above.
 * group_query_groups()
 * group_query_manager()
 * group_query_members
 */
int main(int argc, char** argv)
{
    Test::setup(argc, argv);
    Adapter::pEpLog::set_enabled(true);
    debug_info_full = false;

    // Setup Test Context
    test_create_alice_me();
    test_create_bob_partner();
    test_create_carol_partner();

    // Setup list manager backend
    ListManagerDummy lm_dummy{};
    lm_backend = &lm_dummy;

    logH1("1. Create group");
    ::identity_list* initial_memberlist = nullptr;
    initial_memberlist = new_identity_list(bob);
    ::identity_list_add(initial_memberlist, carol);
    test_group_create(initial_memberlist);

    logH1("2. Add Bob");
    test_group_join(bob); // Fails

    logH1("3. Add Carol");
    test_group_join(carol);

    logH1("4. Remove Carol");
    test_group_remove_member(carol);

    logH1("6. Dissolve");
    test_group_dissolve();

    Adapter::shutdown();
    return 0;
}
