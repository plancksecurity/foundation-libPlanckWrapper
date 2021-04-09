// This file is under GNU General Public License 3.0
// see LICENSE.txt
#include <pEp/group.h>

#include "adapter_group.h"
#include "pEpLog.hh"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************************************
* Group management functions
*************************************************************************************************/

DYNAMIC_API PEP_STATUS adapter_group_create(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *manager,
    identity_list *memberlist,
    pEp_group **group)
{
    pEpLog("called");
    return ::group_create(session, group_identity, manager, memberlist, group);
}

DYNAMIC_API PEP_STATUS
adapter_group_join(PEP_SESSION session, pEp_identity *group_identity, pEp_identity *as_member)
{
    pEpLog("called");
    return ::group_join(session, group_identity, as_member);
}

DYNAMIC_API PEP_STATUS
adapter_group_dissolve(PEP_SESSION session, pEp_identity *group_identity, pEp_identity *manager)
{
    pEpLog("called");
    return ::group_dissolve(session, group_identity, manager);
}

DYNAMIC_API PEP_STATUS adapter_group_invite_member(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *group_member)
{
    pEpLog("called");
    return ::group_invite_member(session, group_identity, group_member);
}

PEP_STATUS adapter_group_remove_member(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *group_member)
{
    pEpLog("called");
    return ::group_remove_member(session, group_identity, group_member);
}

DYNAMIC_API PEP_STATUS adapter_group_rating(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *manager,
    PEP_rating *rating)
{
    pEpLog("called");
    return ::group_rating(session, group_identity, manager, rating);
}

/*************************************************************************************************
 * Group query functions
 *************************************************************************************************/


DYNAMIC_API PEP_STATUS group_query_groups(PEP_SESSION session, identity_list **groups)
{
    pEpLog("called");
    return PEP_STATUS_OK;
}

DYNAMIC_API PEP_STATUS
group_query_manager(PEP_SESSION session, const pEp_identity *const group, pEp_identity **manager)
{
    pEpLog("called");
    return PEP_STATUS_OK;
}

DYNAMIC_API PEP_STATUS
group_query_members(PEP_SESSION session, const pEp_identity *const group, identity_list **members)
{
    pEpLog("called");
    return PEP_STATUS_OK;
}

#ifdef __cplusplus
}
#endif
