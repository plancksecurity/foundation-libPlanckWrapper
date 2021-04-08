// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"
#include "framework/utils.hh"

#include <iostream>

//#include <pEp/keymanagement.h>

#include "../src/Adapter.hh"
#include "../src/adapter_group.h"

using namespace std;
using namespace pEp;

::PEP_STATUS test_messageToSend(::message* _msg)
{
    pEpLog("called");
    cout << Test::make_pEp_msg(Test::make_message(_msg));
    return PEP_STATUS_OK;
}

::PEP_STATUS test_notifyHandshake(::pEp_identity* _me, ::pEp_identity* _partner, sync_handshake_signal signal)
{
    pEpLog("called");
    cout << "me: " << Test::Utils::identity_to_string(_me, false) << endl;
    cout << "partner: " << Test::Utils::identity_to_string(_partner, false) << endl;
    cout << "Signal: " << signal << endl;

    return PEP_STATUS_OK;
}

int main(int argc, char** argv)
{
    Test::setup(argc, argv);
//    Adapter::pEpLog::set_enabled(true);
    bool debug_info_full = true;

    ::pEp_identity* alice = nullptr;
    ::pEp_identity* bob = nullptr;
    ::pEp_identity* carol = nullptr;
    {
        pEpLog("creating identity Alice");
        alice = ::new_identity("alice@peptest.ch", NULL, "23", "Alice");
        assert(alice);
        alice->lang[0] = 'e';
        alice->lang[1] = 'n';
        ::PEP_STATUS status = ::myself(Adapter::session(), alice);
        cout << "Alice:" << Test::Utils::identity_to_string(alice, debug_info_full) << endl;
    }
    {
        pEpLog("creating identity Bob");
        bob = ::new_identity("bob@peptest.ch", NULL, "42", "Bob");
        assert(bob);
        bob->lang[0] = 'c';
        bob->lang[1] = 'r';
        ::PEP_STATUS status = ::myself(Adapter::session(), bob);
        cout << "Bob:" << Test::Utils::identity_to_string(bob, debug_info_full) << endl;
    }
    {
        pEpLog("creating identity Carol");
        carol = ::new_identity("carol@peptest.ch", NULL, "42", "Carol");
        assert(carol);
        carol->lang[0] = 'f';
        carol->lang[1] = 'n';
        ::PEP_STATUS status = ::myself(Adapter::session(), carol);
        cout << "Carol:" << Test::Utils::identity_to_string(carol, debug_info_full) << endl;
    }
    {
        Adapter::sync_initialize(
            Adapter::SyncModes::Async,
            &test_messageToSend,
            &test_notifyHandshake,
            false);
    }
    {
        ::pEp_group* pep_grp1 = nullptr;
        ::pEp_member* mb_bob = ::new_member(bob);
        ::pEp_member* mb_carol = ::new_member(carol);
        cout << "MEMBER: " << endl
             << Test::Utils::member_to_string(mb_carol, debug_info_full) << endl;

        ::member_list* mbl = ::new_memberlist(mb_bob);
        ::memberlist_add(mbl, mb_carol);
        cout << "MBL: " << Test::Utils::memberlist_to_string(mbl, debug_info_full) << endl;

        ::pEp_identity* grp_ident = ::new_identity("group1@peptest.ch", NULL, "12", "group1");
        ::PEP_STATUS status = ::adapter_group_create(Adapter::session(), grp_ident, alice, mbl, &pep_grp1);
        //        cout << "GRP: " << group_to_string(pep_grp1, debug_info_full) << endl;
    }

    Adapter::shutdown();
    return 0;
}
