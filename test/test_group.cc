// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"
#include "framework/utils.hh"

#include <iostream>

#include "../src/Adapter.hh"
#include "../src/adapter_group.h"
#include "../src/status_to_string.hh"

using namespace std;
using namespace pEp;
using namespace pEp::Test::Log;

bool debug_info_full = true;
::pEp_identity* alice = nullptr;
::pEp_identity* bob = nullptr;
::pEp_identity* carol = nullptr;
::pEp_identity* grp_ident = nullptr;
::PEP_STATUS status;

/*
 * Callbacks
 */

::PEP_STATUS test_messageToSend(::message* _msg)
{
    cout << "called" << endl;
    cout << Test::make_pEp_msg(Test::make_message(_msg));
    return PEP_STATUS_OK;
}

::PEP_STATUS test_notifyHandshake(::pEp_identity* _me, ::pEp_identity* _partner, sync_handshake_signal signal)
{
    cout << "called" << endl;
    cout << "me: " << Test::Utils::to_string(_me, false) << endl;
    cout << "partner: " << Test::Utils::to_string(_partner, false) << endl;
    cout << "Signal: " << signal << endl;

    return PEP_STATUS_OK;
}

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
    cout << "Alice:" << Test::Utils::to_string(alice, debug_info_full) << endl;
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
    cout << "Bob:" << Test::Utils::to_string(bob, debug_info_full) << endl;
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
    cout << "Carol:" << Test::Utils::to_string(carol, debug_info_full) << endl;
}

void test_setup_and_start_sync()
{
    logH2("test_setup_and_start_sync");
    Adapter::sync_initialize(Adapter::SyncModes::Async, &test_messageToSend, &test_notifyHandshake, false);
}

void test_group_create(::identity_list* idl)
{
    logH2("test_group_create");
    cout << "IDL: " << Test::Utils::to_string(idl, debug_info_full) << endl;

    cout << "create group identity" << endl;
    grp_ident = ::new_identity("group1@peptest.ch", NULL, "432", "group1");
    assert(grp_ident);
    status = ::myself(Adapter::session(), grp_ident);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "grp_ident:" << Test::Utils::to_string(grp_ident, debug_info_full) << endl;

    cout << "adapter_group_create()" << endl;
    ::pEp_group* pep_grp1 = nullptr;
    status = ::adapter_group_create(Adapter::session(), grp_ident, alice, idl, &pep_grp1);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    assert(pep_grp1);
    cout << "GRP: " << Test::Utils::to_string(pep_grp1, debug_info_full) << endl;
}

void test_group_invite_member(::pEp_identity* ident)
{
    logH2("test_group_invite_member");
    status = ::adapter_group_invite_member(Adapter::session(), grp_ident, ident);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
}

void test_group_join(::pEp_identity* ident)
{
    logH2("test_group_join");
    status = ::adapter_group_join(Adapter::session(), grp_ident, ident);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
}

void test_group_remove_member(::pEp_identity* ident)
{
    logH2("test_group_remove_member");
    status = ::adapter_group_remove_member(Adapter::session(), grp_ident, ident);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
}

void test_group_rating()
{
    logH2("test_group_rating");
    // Rating
    ::PEP_rating* rating = nullptr;
    cout << "adapter_group_rating()" << endl;
    status = ::group_rating(Adapter::session(), grp_ident, alice, rating);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "Rating: " << rating << endl;
}

void test_group_dissolve()
{
    logH2("test_group_dissolve");
    status = ::adapter_group_dissolve(Adapter::session(), grp_ident, alice);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
}

/*
 * Update functions
 * ----------------
 * Test procedure:
 * 1. Create group
 * - group_create(Alice)
 * 2. Add Bob
 * - group_invite_member(Bob)
 * - group_join(Bob)
 * 3. Add Carol
 * - group_invite_member(Carol)
 * - group_join(Carol)
 * 4. Remove Carol
 * - group_remove_member(Carol)
 * 5. Rating
 * - group_rating() (Just test once, to see it is generally working)
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
    Adapter::pEpLog::set_enabled(false);
    debug_info_full = true;

    // Setup Test Context
    test_create_alice_me();
    test_create_bob_partner();
    test_create_carol_partner();
    test_setup_and_start_sync();


    logH1("1. Create group");
    ::identity_list* initial_memberlist = nullptr;
    initial_memberlist = new_identity_list(bob);
    ::identity_list_add(initial_memberlist, carol);
    test_group_create(initial_memberlist);
    logH1("2. Add Bob");
    //    test_group_invite_member(bob); // Fails
    //    test_group_join(bob); // Fails
    logH1("3. Add Carol");
    //    test_group_invite_member(carol);
    //    test_group_join(carol);
    logH1("4. Remove Carol");
    //    test_group_remove_member(carol);
    logH1("5. Rating");
    //    test_group_rating(); // Failing
    logH1("6. Dissolve");
    test_group_dissolve();

    Adapter::shutdown();
    return 0;
}
