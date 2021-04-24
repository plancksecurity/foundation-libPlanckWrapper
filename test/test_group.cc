// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"
#include "framework/utils.hh"

//#include <iostream>

#include "../src/Adapter.hh"
#include "../src/utils.hh"

#include "../src/grp_update_interface.hh"
#include "../src/grp_update_drv_engine.hh"
#include "../src/grp_update_drv_dummy.hh"
//#include "../src/adapter_group.h"
#include "../src/status_to_string.hh"

#include <pEp/pEpEngine.h>
#include <pEp/message_api.h>

using namespace std;
using namespace pEp;
using namespace pEp::Adapter::pEpLog;

bool debug_info_full = true;
::pEp_identity* alice = nullptr;
::pEp_identity* bob = nullptr;
::pEp_identity* carol = nullptr;
::pEp_identity* grp_ident = nullptr;
::PEP_STATUS status;

string dummy_in;

GroupUpdateInterface* gu = nullptr;

/*
 * Callbacks
 */

::PEP_STATUS test_messageToSend(::message* _msg)
{
    log("called");
    log(Test::make_pEp_msg(Test::make_message(_msg)));
    return PEP_STATUS_OK;
}

::PEP_STATUS test_notifyHandshake(::pEp_identity* _me, ::pEp_identity* _partner, sync_handshake_signal signal)
{
    log("called");
    log("me: " + pEp::Utils::to_string(_me, false));
    log("partner: " + pEp::Utils::to_string(_partner, false));
    log("Signal: " + string{ ::sync_handshake_signal_to_string(signal) });

    return PEP_STATUS_OK;
}

/*
 * Test Units
 */

void test_create_alice_me()
{
    //    Adapter::session(pEp::Adapter::release);
    logH2("test_create_alice_me");
    alice = ::new_identity("alice@peptest.ch", NULL, "23", "Alice");
    assert(alice);
    alice->lang[0] = 'e';
    alice->lang[1] = 'n';
    status = ::myself(Adapter::session(), alice);
    log("STATUS: " + status_to_string(status));
    assert(!status);
    log("Alice:" + pEp::Utils::to_string(alice, debug_info_full));
}

void test_create_bob_partner()
{
    logH2("test_create_bob_partner");
    bob = ::new_identity("bob@peptest.ch", NULL, PEP_OWN_USERID, "Bob");
    assert(bob);
    bob->lang[0] = 'c';
    bob->lang[1] = 'r';
    status = ::update_identity(Adapter::session(), bob);
    log("STATUS: " + status_to_string(status));
    assert(!status);
    log("Bob:" + pEp::Utils::to_string(bob, debug_info_full));
}

void test_create_carol_partner()
{
    logH2("test_create_carol_partner");
    carol = ::new_identity("carol@peptest.ch", NULL, PEP_OWN_USERID, "Carol");
    assert(carol);
    carol->lang[0] = 'f';
    carol->lang[1] = 'n';
    status = ::update_identity(Adapter::session(), carol);
    log("STATUS: " + status_to_string(status));
    assert(!status);
    log("Carol:" + pEp::Utils::to_string(carol, debug_info_full));
}

void test_setup_and_start_sync()
{
    logH2("test_setup_and_start_sync");
    Adapter::sync_initialize(Adapter::SyncModes::Async, &test_messageToSend, &test_notifyHandshake, false);
}

void test_group_create(::identity_list& idl)
{
    logH2("test_group_create");
    log("IDL: " + pEp::Utils::to_string(&idl, debug_info_full));

    log("create group identity");
    grp_ident = ::new_identity("group1@peptest.ch", NULL, "432", "group1");
    assert(grp_ident);
    status = ::myself(Adapter::session(), grp_ident);
    log("STATUS: " + status_to_string(status));
    assert(!status);
    log("grp_ident:" + pEp::Utils::to_string(grp_ident, debug_info_full));

    log("adapter_group_create()");
    status = gu->adapter_group_create(Adapter::session(), grp_ident, alice, &idl);
    log("STATUS: " + status_to_string(status));
    assert(!status);
}

void test_group_invite_member(::pEp_identity& ident)
{
    logH2("test_group_invite_member");
    assert(grp_ident);
    status = gu->adapter_group_invite_member(Adapter::session(), grp_ident, &ident);
    log("STATUS: " + status_to_string(status));
    assert(!status);
}

//void test_group_join(::pEp_identity& ident)
//{
//    logH2("test_group_join");
//    status = gu->adapter_group_join(Adapter::session(), grp_ident, &ident);
//    log("STATUS: " + status_to_string(status));
//    assert(!status);
//}

void test_group_remove_member(::pEp_identity& ident)
{
    logH2("test_group_remove_member");
    status = gu->adapter_group_remove_member(Adapter::session(), grp_ident, &ident);
    log("STATUS: " + status_to_string(status));
    assert(!status);
}

void test_group_rating()
{
    logH2("test_group_rating");
    // Rating
    ::PEP_rating rating;
    log("adapter_group_rating()");
    status = ::group_rating(Adapter::session(), grp_ident, alice, &rating);
    log("STATUS: " + status_to_string(status));
    assert(!status);
    log("Rating: " + string{::rating_to_string(rating)});
}

void test_group_dissolve()
{
    logH2("test_group_dissolve");
    status = gu->adapter_group_dissolve(Adapter::session(), grp_ident, alice);
    log("STATUS: " + status_to_string(status));
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

    GroupUpdateDriverDummy gud{ "test.db" };
    GroupUpdateDriverEngine gue{};

    //    gu = &gud;
    gu = &gue;

    // Setup Test Context
    test_create_alice_me();
    log("PERUSERDIR: " + string(::per_user_directory()));
    //    pEp::Utils::file_delete(::per_user_directory());
    //    cin >> dummy_in;

    test_create_alice_me();

    test_create_bob_partner();
    test_create_carol_partner();
    test_setup_and_start_sync();

    logH1("1. Create group");
    ::identity_list* initial_memberlist = nullptr;
    initial_memberlist = new_identity_list(bob);
    ::identity_list_add(initial_memberlist, carol);
    test_group_create(*initial_memberlist);
    log("USER DIR: " + string{ ::per_user_directory() });
    logH1("2. Add Bob");
    test_group_invite_member(*bob); // Fails
                                    //        test_group_join(bob); // Fails
    logH1("3. Add Carol");
    //    test_group_invite_member(carol);
    //    test_group_join(carol);
    logH1("4. Remove Carol");
    test_group_remove_member(*carol);
    logH1("5. Rating");
    test_group_rating();
    logH1("6. Dissolve");
    test_group_dissolve();


    Adapter::shutdown();
    return 0;
}
