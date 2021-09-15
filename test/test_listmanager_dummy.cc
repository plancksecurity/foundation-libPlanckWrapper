#include "../src/listmanager_dummy.hh"
#include "../src/utils.hh"
#include "framework/test_utils.hh"
#include <exception>
#include <map>
#include <cassert>


using namespace std;
using namespace pEp;
using namespace pEp::Adapter::pEpLog;
using namespace pEp::Utils;

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
    vector<string> lists_addr() const
    {
        vector<string> ret;
        for (const lm_list& l : this->lists) {
            ret.push_back(l.addr);
        }
        return ret;
    }
};

void apply_model(ListManagerDummy& lmd, const model_test_lmd& model)
{
//    log("apply_model()");
    for (const lm_list& l : model.lists) {
        lmd.list_add(l.addr, l.mod);
        for (const string& m : l.members) {
            lmd.member_add(l.addr, m);
        }
    }
}

void verify_model(ListManagerDummy& lmd, const model_test_lmd& model)
{
//    log("verify_model()");
    assert((model.lists_addr()) == lmd.lists());
    for (const lm_list& l : model.lists) {
        assert(l.members == lmd.members(l.addr));
    }
    for (const lm_list& l : model.lists) {
        assert(l.mod == lmd.moderator(l.addr));
    }
}

void recreate_apply_and_verify_model(ListManagerDummy& lmd, const model_test_lmd& model)
{
    try {
        lmd.delete_db();
    } catch (const exception& e) {
    }
    assert(!path_exists(model.db_path));
    apply_model(lmd, model);
    verify_model(lmd, model);
}

model_test_lmd create_default_model()
{
    model_test_lmd model;
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
    return model;
}

model_test_lmd create_model_bad_path()
{
    model_test_lmd model = create_default_model();
    model.db_path = "/wont_create_dirs/bad.db";
    return model;
}

int main(int argc, char* argv[])
{
    //        pEpSQLite::log_enabled = true;
    ListManagerDummy::log_enabled = false;

    logH1("Testing SUCCESS conditions");
    {
        logH2("Test create new db");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
    }
    {
        logH2("Test re-open db");
        model_test_lmd model = create_default_model();
        assert(path_exists(model.db_path));
        ListManagerDummy lmd(model.db_path);
        verify_model(lmd, model);

        logH2("Test list_delete");
        lmd.list_delete(model.lists.at(2).addr);
        model.lists.pop_back();
        verify_model(lmd, model);

        logH2("Test auto reopen after close()");
        lmd.close_db();

        logH2("Test member_remove");
        lmd.member_remove(model.lists.at(0).addr, model.lists.at(0).members.at(1));
        model.lists.at(0).members.pop_back();
        verify_model(lmd, model);

        logH2("Test list_exists() - true");
        assert(lmd.list_exists(model.lists.at(0).addr));

        logH2("Test list_exists() - false");
        assert(!lmd.list_exists("does_not_exist_for_sure"));

        logH2("Test member_exists() - true");
        assert(lmd.member_exists(model.lists.at(0).addr, model.lists.at(0).members.at(0)));

        logH2("Test member_exists() - false");
        assert(!lmd.member_exists(model.lists.at(0).addr, "does_not_exist_for_sure"));

        logH2("Test delete_db");
        lmd.delete_db();
        assert(!path_exists(model.db_path));
    }

    logH1("Testing ERROR conditions");
    {
        logH2("Testing success on close_db() on model_bad_path");
        model_test_lmd model = create_model_bad_path();
        ListManagerDummy lmd(model.db_path);
        lmd.close_db();
    }
    {
        logH2("Testing exception on delete_db() on model_bad_path");
        model_test_lmd model = create_model_bad_path();
        ListManagerDummy lmd(model.db_path);
        ASSERT_EXCEPT(lmd.delete_db());
    }
    {
        logH2("Testing exception on lists() on: on model_bad_path");
        model_test_lmd model = create_default_model();
        model.db_path = "/wont_create_dirs/bad.db";
        ListManagerDummy lmd(model.db_path);
        ASSERT_EXCEPT(lmd.lists());
    }
    // ------------------------------------------------------------------------------------
    logH1("list_add() Error conditions");
    {
        logH2("Testing list_add() AlreadyExistsException");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
        try {
            lmd.list_add(model.lists.at(0).addr, "any");
            assert(false);
        } catch (const AlreadyExistsException& e) {
            log(nested_exception_to_string(e));
        } catch (...) {
            assert(false);
        }
    }
    // ------------------------------------------------------------------------------------
    logH1("list_delete() Error conditions");
    {
        logH2("Testing list_delete() DoesNotExistException");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
        try {
            lmd.list_delete("does_not_exist_for_sure");
            assert(false);
        } catch (const ListDoesNotExistException& e) {
            log(nested_exception_to_string(e));
        } catch (...) {
            assert(false);
        }
    }
    // ------------------------------------------------------------------------------------
    logH1("member_add() Error conditions");
    {
        logH2("Testing member_add() AlreadyExistsException");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
        try {
            lmd.member_add(model.lists.at(0).addr, model.lists.at(0).members.at(0));
            assert(false);
        } catch (const AlreadyExistsException& e) {
            log(nested_exception_to_string(e));
        } catch (...) {
            assert(false);
        }
    }
    {
        logH2("Testing member_add() to not existing list - DoesNotExistException");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
        try {
            lmd.member_add("does_not_exist_for_sure", model.lists.at(0).members.at(0));
            assert(false);
        } catch (const ListDoesNotExistException& e) {
            log(nested_exception_to_string(e));
        } catch (...) {
            assert(false);
        }
    }
    // ------------------------------------------------------------------------------------
    logH1("member_remove() Error conditions");
    {
        logH2("Testing member_remove() not existing member - DoesNotExistException");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
        try {
            lmd.member_remove(model.lists.at(0).addr, "does_not_exist_for_sure");
            assert(false);
        } catch (const MemberDoesNotExistException& e) {
            log(nested_exception_to_string(e));
        } catch (...) {
            assert(false);
        }
    }
    {
        logH2("Testing member_remove() not existing list - DoesNotExistException");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
        try {
            lmd.member_remove("does_not_exist_for_sure", model.lists.at(0).members.at(0));
            assert(false);
        } catch (const ListDoesNotExistException& e) {
            log(nested_exception_to_string(e));
        } catch (...) {
            assert(false);
        }
    }
    // ------------------------------------------------------------------------------------
    logH1("moderator() Error conditions");
    {
        logH2("Testing moderator() DoesNotExistException");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
        try {
            lmd.moderator("does_not_exist_for_sure");
            assert(false);
        } catch (const ListDoesNotExistException& e) {
            log(nested_exception_to_string(e));
        } catch (...) {
            assert(false);
        }
    }
    // ------------------------------------------------------------------------------------
    logH1("members() Error conditions");
    {
        logH2("Testing members() DoesNotExistException");
        model_test_lmd model = create_default_model();
        ListManagerDummy lmd(model.db_path);
        recreate_apply_and_verify_model(lmd, model);
        try {
            lmd.members("does_not_exist_for_sure");
            assert(false);
        } catch (const ListDoesNotExistException& e) {
            log(nested_exception_to_string(e));
        } catch (...) {
            assert(false);
        }
    }

    logH1("All Tests SUCCESSFUL");
}