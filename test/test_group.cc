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
 * 6. Disssolve
 * - group_dissolve()
 *
 * Query functions
 * ---------------
 * Always test all the query functions for correctness between every step above.
 * group_query_groups()
 * group_query_manager()
 * group_query_members
 */


bool debug_info_full = true;
::pEp_identity* alice = nullptr;
::pEp_identity* bob = nullptr;
::pEp_identity* carol = nullptr;
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
    cout << "me: " << Test::Utils::identity_to_string(_me, false) << endl;
    cout << "partner: " << Test::Utils::identity_to_string(_partner, false) << endl;
    cout << "Signal: " << signal << endl;

    return PEP_STATUS_OK;
}

/*
 * Test Units
 */

void create_alice_me()
{
    cout << "======== Alice myself()" << endl;
    alice = ::new_identity("alice@peptest.ch", NULL, "23", "Alice");
    assert(alice);
    alice->lang[0] = 'e';
    alice->lang[1] = 'n';
    status = ::myself(Adapter::session(), alice);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "Alice:" << Test::Utils::identity_to_string(alice, debug_info_full) << endl;
}

void create_bob_partner()
{
    cout << "======== Bob update_identity()" << endl;
    bob = ::new_identity("bob@peptest.ch", NULL, PEP_OWN_USERID, "Bob");
    assert(bob);
    bob->lang[0] = 'c';
    bob->lang[1] = 'r';
    status = ::update_identity(Adapter::session(), bob);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "Bob:" << Test::Utils::identity_to_string(bob, debug_info_full) << endl;
}

void create_carol_partner()
{
    cout << "======== Carol update_identity()" << endl;
    carol = ::new_identity("carol@peptest.ch", NULL, PEP_OWN_USERID, "Carol");
    assert(carol);
    carol->lang[0] = 'f';
    carol->lang[1] = 'n';
    status = ::update_identity(Adapter::session(), carol);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "Carol:" << Test::Utils::identity_to_string(carol, debug_info_full) << endl;
}

void setup_and_start_sync()
{
    cout << "======== sync_initialize()" << endl;
    Adapter::sync_initialize(Adapter::SyncModes::Async, &test_messageToSend, &test_notifyHandshake, false);
}

void group_rating()
{
    cout << "======== 5. Rating" << endl;
    // recreate group identity
    ::pEp_identity* grp_ident = ::new_identity("group1@peptest.ch", NULL, NULL, NULL);
    assert(grp_ident);
    status = ::update_identity(Adapter::session(), grp_ident);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "grp_ident:" << Test::Utils::identity_to_string(grp_ident, debug_info_full) << endl;

    // Rating
    ::PEP_rating* rating = nullptr;
    cout << "adapter_group_rating()" << endl;
    status = ::group_rating(Adapter::session(), grp_ident, alice, rating);
    cout << "STATUS: " << status_to_string(status) << endl;
    cout << "Rating: " << rating << endl;
    assert(!status);
}

void create_group()
{
    cout << "========  1. Create group" << endl;
    ::pEp_group* pep_grp1 = nullptr;

    // Initial Memberlist
    ::identity_list* idl = new_identity_list(bob);
    ::identity_list_add(idl, carol);
    cout << "IDL: " << Test::Utils::identitylist_to_string(idl, debug_info_full) << endl;

    // Group Identity
    ::pEp_identity* grp_ident = ::new_identity("group1@peptest.ch", NULL, "432", "group1");
    assert(grp_ident);
    status = ::myself(Adapter::session(), grp_ident);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    cout << "grp_ident:" << Test::Utils::identity_to_string(grp_ident, debug_info_full) << endl;

    // Create group
    cout << "adapter_group_create()" << endl;
    status = ::adapter_group_create(Adapter::session(), grp_ident, alice, idl, &pep_grp1);
    cout << "STATUS: " << status_to_string(status) << endl;
    assert(!status);
    assert(pep_grp1);
    cout << "GRP: " << Test::Utils::group_to_string(pep_grp1, debug_info_full) << endl;
}


/*
 * Test Procedure
 * It has the advantage that you can comment out one line if a test fails
 */

int main(int argc, char** argv)
{
    Test::setup(argc, argv);
    Adapter::pEpLog::set_enabled(false);
    debug_info_full = false;
    create_alice_me();
    create_bob_partner();
    create_carol_partner();
    setup_and_start_sync();
    create_group();
//    group_rating(); // Failing

    Adapter::shutdown();
    return 0;
}
