// This file is under GNU General Public License 3.0
// see LICENSE.txt
//#include <pEp/group.h>
// clang-format off
#include "group_manager_api.h"
#include <pEp/pEpLog.hh>
#include "grp_driver_engine.hh"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
using namespace pEp;

Adapter::GroupDriverEngine adapter_grp_manager{};

DYNAMIC_API PEP_STATUS adapter_group_init()
{
    PEP_STATUS status = PEP_STATUS_OK;
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
    pEp_identity *as_member,
    pEp_identity* manager
)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.adapter_group_join(session, group_identity, as_member, manager);
    return status;
}

/*************************************************************************************************
 * Group query functions
 *************************************************************************************************/
DYNAMIC_API PEP_STATUS adapter_group_query_groups_as_manager(
    PEP_SESSION session,
    pEp_identity *manager,
    identity_list **groups)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.group_query_groups_as_manager(session, manager, groups);
    return status;
}

DYNAMIC_API PEP_STATUS adapter_group_query_manager(
    PEP_SESSION session,
    pEp_identity *group,
    pEp_identity **manager)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.group_query_manager(session, group, manager);
    return status;
}

DYNAMIC_API PEP_STATUS adapter_group_query_members(
    PEP_SESSION session,
    pEp_identity *group,
    identity_list **members)
{
    pEpLog("called");
    PEP_STATUS status = adapter_grp_manager.group_query_members(session, group, members);
    return status;
}

#ifdef __cplusplus
}
#endif
