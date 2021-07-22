// This file is under GNU General Public License 3.0
// see LICENSE.txt
//#include <pEp/group.h>
// clang-format off
#include "group_manager_api.h"
#include "grp_driver_replicator.hh"
#include "pEpLog.hh"
#include "grp_driver_dummy.hh"
#include "grp_driver_engine.hh"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
using namespace pEp;

Adapter::GroupDriverReplicator adapter_grp_manager{};
shared_ptr<Adapter::GroupDriverDummy> grp_drv_dummy;
shared_ptr<Adapter::GroupDriverEngine> grp_drv_engine;

DYNAMIC_API PEP_STATUS adapter_group_init()
{
    PEP_STATUS status = PEP_STATUS_OK;
    try {
        const string lm_dummy_db_filename = "groups.db";
#ifdef WIN32
        const string lm_dummy_db_path = string(::per_user_directory()) + "\\" + lm_dummy_db_filename;
#else
        const string lm_dummy_db_path = string(::per_user_directory()) + "/" + lm_dummy_db_filename;
#endif

        if(!grp_drv_dummy) {
            grp_drv_dummy = make_shared<Adapter::GroupDriverDummy>(lm_dummy_db_path);
        }
        if(!grp_drv_engine) {
            grp_drv_engine = make_shared<Adapter::GroupDriverEngine>();
        }
        adapter_grp_manager.set_replication_source(*grp_drv_dummy.get());
        adapter_grp_manager.set_replication_destination(*grp_drv_engine.get());
    } catch (const std::exception &e) {
        pEpLog(Utils::nested_exception_to_string(e));
        status = PEP_UNKNOWN_ERROR;
    } catch (...) {
        pEpLog("unknown exception");
        status = PEP_UNKNOWN_ERROR;
    }
    return status;
}

/*************************************************************************************************
* Group management functions
*************************************************************************************************/
DYNAMIC_API PEP_STATUS adapter_group_create(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *manager,
    identity_list *memberlist)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.adapter_group_create(
        session,
        group_identity,
        manager,
        memberlist);
    return status;
}


DYNAMIC_API PEP_STATUS adapter_group_dissolve(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *manager)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.adapter_group_dissolve(session, group_identity, manager);
    return status;
}

DYNAMIC_API PEP_STATUS adapter_group_invite_member(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *group_member)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.adapter_group_invite_member(
        session,
        group_identity,
        group_member);
    return status;
}

DYNAMIC_API PEP_STATUS adapter_group_remove_member(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *group_member)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.adapter_group_remove_member(
        session,
        group_identity,
        group_member);
    return status;
}

DYNAMIC_API PEP_STATUS adapter_group_join(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *as_member)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.adapter_group_join(session, group_identity, as_member);
    return status;
}

/*************************************************************************************************
 * Group query functions
 *************************************************************************************************/
DYNAMIC_API PEP_STATUS adapter_group_query_groups(
    PEP_SESSION session,
    identity_list **groups)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.group_query_groups(session, groups);
    return status;
}

DYNAMIC_API PEP_STATUS adapter_group_query_manager(
    PEP_SESSION session,
    const pEp_identity *const group,
    pEp_identity **manager)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.group_query_manager(session, group, manager);
    return status;
}

DYNAMIC_API PEP_STATUS adapter_group_query_members(
    PEP_SESSION session,
    const pEp_identity *const group,
    identity_list **members)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.group_query_members(session, group, members);
    return status;
}

#ifdef __cplusplus
}
#endif
