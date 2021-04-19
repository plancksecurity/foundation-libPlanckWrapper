#include "../src/listmanager_dummy.hh"
#include "framework/utils.hh"
#include <iostream>
#include <exception>
#include <map>

using namespace std;
using namespace pEp;
using namespace Test::Log;
using namespace Test::Utils;

struct lm_list {
    string addr;
    string mod;
    vector<string> members;
};

struct model_test_lmd {
    string alice;
    string bob;
    string carol;
    string joe;
    string db_path;
    vector<lm_list> lists;
};

model_test_lmd model;


vector<string> list_of_lists(const vector<lm_list>& lists)
{
    vector<string> ret;
    for (lm_list l : lists) {
        ret.push_back(l.addr);
    }
    return ret;
}

void apply_model(ListManagerDummy& lmd)
{
    logH2("Adding model");
    for (lm_list l : model.lists) {
        lmd.list_add(l.addr, l.mod);
        for (string m : l.members) {
            lmd.member_add(l.addr, m);
        }
    }
}

void verify_model(ListManagerDummy& lmd)
{
    logH2("Verifying model");
    cout << "Verifying lists" << endl;
    assert(list_of_lists(model.lists) == lmd.lists());
    cout << "Verifying members" << endl;
    for (lm_list l : model.lists) {
        assert(l.members == lmd.members(l.addr));
    }
    cout << "Verifying moderators" << endl;
    for (lm_list l : model.lists) {
        assert(l.mod == lmd.moderator(l.addr));
    }
}

void recreate_apply_and_verify_model(ListManagerDummy& lmd)
{
    logH2("test delete_db()");
    try {
        lmd.delete_db();
    } catch (const exception& e) {
        print_exception(e);
    }
    assert(!file_exists(model.db_path));
    apply_model(lmd);
    verify_model(lmd);
}

int main(int argc, char* argv[])
{
    try {
        ListManagerDummy::log_enabled = true;

        {
            model.db_path = "test_lmd.db";
            model.alice = "alice@peptest.org";
            model.bob = "bob@peptest.org";
            model.carol = "carol@peptest.org";
            model.joe = "joe@peptest.org";

            lm_list l1;
            l1.addr = "list1@peptest.org";
            l1.mod = model.alice;
            l1.members.push_back(model.bob);
            l1.members.push_back(model.carol);

            lm_list l2;
            l2.addr = "list2@peptest.org";
            l2.mod = model.alice;
            l2.members.push_back(model.bob);
            l2.members.push_back(model.carol);
            l2.members.push_back(model.joe);

            lm_list l3;
            l3.addr = "list3@peptest.org";
            l3.mod = model.bob;
            l3.members.push_back(model.carol);
            l3.members.push_back(model.joe);

            model.lists.push_back(l1);
            model.lists.push_back(l2);
            model.lists.push_back(l3);
        }

        {
            logH1("Test create new db");
            ListManagerDummy lmd(model.db_path);
            recreate_apply_and_verify_model(lmd);
        }
        {
            logH1("Test re-open db");
            assert(file_exists(model.db_path));
            ListManagerDummy lmd(model.db_path);
            verify_model(lmd);

            logH1("Test list_delete");
            lmd.list_delete(model.lists.at(2).addr);
            model.lists.pop_back();
            verify_model(lmd);

            logH1("Test auto reopen after close()");
            lmd.close_db();

            logH1("Test member_remove");
            lmd.member_remove(model.lists.at(0).addr, model.lists.at(0).members.at(1));
            model.lists.at(0).members.pop_back();
            verify_model(lmd);

            logH1("Test delete_db");
            lmd.delete_db();
            assert(!file_exists(model.db_path));
        }
        {
            logH1("Testing error conditions");
            ListManagerDummy lmd(model.db_path);
            recreate_apply_and_verify_model(lmd);

            //TODO: Continue with error conditions
            // * bad path
            // * list_add - list already exists
            // * list_delete - no such list
            // * member_add - member already exists
            // * member_remove - no such member
            // * moderator - no such list
            // * members - no such list
        }

    } catch (const exception& e) {
        print_exception(e);
    }
}