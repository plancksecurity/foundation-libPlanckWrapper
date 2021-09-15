// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"
#include <pEp/pitytest11/test_utils.hh>

//#include <iostream>

#include "../src/Adapter.hh"
#include "../src/utils.hh"
#include "../src/std_utils.hh"

#include "../src/grp_manager_interface.hh"
#include "../src/grp_driver_engine.hh"
#include "../src/grp_driver_dummy.hh"
#include "../src/grp_driver_replicator.hh"

//#include "../src/adapter_group.h"
#include "../src/status_to_string.hh"

#include <pEp/pEpEngine.h>
#include <pEp/message_api.h>

using namespace std;
using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::Adapter::pEpLog;

bool debug_info_full = true;


// Model
const string lmd_path = "test.db";
::pEp_identity* alice = nullptr;
::pEp_identity* bob = nullptr;
::pEp_identity* carol = nullptr;
::pEp_identity* grp_ident = nullptr;
::PEP_STATUS status;


string dummy_in;


GroupUpdateInterface* gu = nullptr;
GroupQueryInterface* gq = nullptr;

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
    log("Signal: " + to_string(signal));
    //    log("Signal: " + string{ ::sync_handshake_signal_to_string(signal) });

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
    log("STATUS: " + status_to_string(status));
    assert(!status);
    log("Alice:" + pEp::Utils::to_string(alice, debug_info_full));
}

void test_create_bob_partner()
{
    logH2("test_create_bob_partner");
    bob = ::new_identity("bob@peptest.ch", NULL, PEP_OWN_USERID, "Bob");
    assert(bob);
//    bob->lang[0] = 'c';
//    bob->lang[1] = 'r';
//    status = ::myself(Adapter::session(), bob);
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
//    status = ::myself(Adapter::session(), carol);
    log("STATUS: " + status_to_string(status));
    assert(!status);
    log("Carol:" + pEp::Utils::to_string(carol, debug_info_full));
}

void test_setup_and_start_sync()
{
    logH2("test_setup_and_start_sync");
    Adapter::start_sync();
}

void test_group_create()
{
    logH2("test_group_create");
    ::identity_list* initial_memberlist = nullptr;
    initial_memberlist = new_identity_list(bob);
    ::identity_list_add(initial_memberlist, carol);

    log("create group identity");
    grp_ident = ::new_identity("group1@peptest.ch", NULL, "432", "group1");
    assert(grp_ident);
    log("grp_ident:" + pEp::Utils::to_string(grp_ident, debug_info_full));

//    PEP_STATUS stat = ::myself(Adapter::session(), grp_ident);
//    log("STATUS: " + status_to_string(status));
//    assert(stat == PEP_STATUS_OK);
//    log("grp_ident:" + pEp::Utils::to_string(grp_ident, debug_info_full));

    log("adapter_group_create()");
    status = gu->adapter_group_create(Adapter::session(), grp_ident, alice, nullptr);
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

void test_group_remove_member(::pEp_identity& ident)
{
    logH2("test_group_remove_member");
    status = gu->adapter_group_remove_member(Adapter::session(), grp_ident, &ident);
    log("STATUS: " + status_to_string(status));
    assert(!status);
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
// * - group_join(Bob)
 * 3. Add Carol
 * - group_invite_member(Carol)
// * - group_join(Carol)
 * 4. Remove Carol
 * - group_remove_member(Carol)
// * 5. Rating
// * - group_rating() (Just test once, to see it is generally working)
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

    //    pEpSQLite::log_enabled = true;
    //    ListManagerDummy::log_enabled = true;
    GroupDriverDummy::log_enabled = true;
    GroupDriverEngine::log_enabled = true;
    GroupDriverReplicator::log_enabled = true;


    GroupDriverDummy gdd{ lmd_path };
    GroupDriverEngine gde{};
    GroupDriverReplicator gdr{};


    gu = &gde;
    gq = nullptr;

    //    gu = &gdd;
    //    gq = &gdd;

    //    gu = &gdr;
    //    gq = &gdr;
    Adapter::session.initialize();

    // Setup Test Context
    test_create_alice_me();
    test_create_bob_partner();
    test_create_carol_partner();

    test_setup_and_start_sync();

    logH1("1. Create group");

    test_group_create();
    logH1("2. Add Bob");
    test_group_invite_member(*bob); // Fails
    logH1("3. Add Carol");
    test_group_invite_member(*carol);
    logH1("4. Remove Carol");
    test_group_remove_member(*carol);

    if (gq != nullptr) {
        ::pEp_identity* grp_ident = ::new_identity("group1@peptest.ch", NULL, "432", "group1");
        ::pEp_identity* manager = nullptr;
        PEP_STATUS stat = gq->group_query_manager(Adapter::session(), grp_ident, &manager);
        log(status_to_string(stat));
        log(::Utils::to_string(manager));
    }
    logH1("6. Dissolve");
    test_group_dissolve();


    Adapter::stop_sync();
    return 0;
}
