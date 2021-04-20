// This file is under GNU General Public License 3.0
// see LICENSE.txt
#include <pEp/group.h>

#include "adapter_group.h"
#include "grp_update_interface.hh"
#include "grp_query_interface.hh"
#include "grp_update_drv_engine.hh"
#include "grp_update_drv_dummy.hh"
#include "grp_query_drv_dummy.hh"
#include "pEpLog.hh"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;
using namespace pEp;

//Listmanager config
string lm_dummy_db_path = "listman_dummy.db";

/*************************************************************************************************
* Group management functions
*************************************************************************************************/
// Engine Driver
GroupUpdateDriverEngine group_update_driver_engine{};
GroupUpdateInterface *gu_engine = &group_update_driver_engine;
// Listmanager Driver
GroupUpdateDriverDummy group_update_driver_dummy{lm_dummy_db_path};
GroupUpdateInterface *gu_listman = &group_update_driver_dummy;

DYNAMIC_API PEP_STATUS adapter_group_create(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *manager,
    identity_list *memberlist,
    pEp_group **group)
{
    pEpLog("called");

    // Do listmanager
    PEP_STATUS status = gu_listman->adapter_group_create(
        session,
        group_identity,
        manager,
        memberlist,
        group);

    if (status != PEP_STATUS_OK) {
        return status;
    }

    // Do engine
    status = gu_engine->adapter_group_create(session, group_identity, manager, memberlist, group);
    if (status != PEP_STATUS_OK) {
        // Rollback listman
        PEP_STATUS rb_stat = gu_listman->adapter_group_dissolve(session, group_identity, manager);
        if (rb_stat != PEP_STATUS_OK) {
            //FATAL ERROR ON ROLLBACK
            status = (PEP_STATUS)-9999;
        }
    }
    return status;
}

DYNAMIC_API PEP_STATUS
adapter_group_join(PEP_SESSION session, pEp_identity *group_identity, pEp_identity *as_member)
{
    pEpLog("called");
    // Do listmanager
    PEP_STATUS status = gu_listman->adapter_group_join(session, group_identity, as_member);
    if (status != PEP_STATUS_OK) {
        return status;
    }

    // Do engine
    status = gu_engine->adapter_group_join(session, group_identity, as_member);
    if (status != PEP_STATUS_OK) {
        // Rollback listman
        //TODO: ????
        //        PEP_STATUS rb_stat = gu_listman->adapter_group_dissolve(session, group_identity, manager);
        //        if (rb_stat != PEP_STATUS_OK) {
        //            //FATAL ERROR ON ROLLBACK
        //            status = (PEP_STATUS)-9999;
        //        }
    }
    return status;
}

DYNAMIC_API PEP_STATUS
adapter_group_dissolve(PEP_SESSION session, pEp_identity *group_identity, pEp_identity *manager)
{
    pEpLog("called");
    // Do listmanager
    PEP_STATUS status = gu_listman->adapter_group_dissolve(session, group_identity, manager);
    if (status != PEP_STATUS_OK) {
        return status;
    }

    // Do engine
    status = gu_engine->adapter_group_dissolve(session, group_identity, manager);
    if (status != PEP_STATUS_OK) {
        // Rollback listman
        // TODO: ????
        //        PEP_STATUS rb_stat = gu_listman->adapter_group_dissolve(session, group_identity, manager);
        //        if (rb_stat != PEP_STATUS_OK) {
        //            //FATAL ERROR ON ROLLBACK
        //            status = (PEP_STATUS)-9999;
        //        }
    }
    return status;
}

DYNAMIC_API PEP_STATUS adapter_group_invite_member(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *group_member)
{
    pEpLog("called");
    // Do listmanager
    PEP_STATUS status = gu_listman->adapter_group_invite_member(session, group_identity, group_member);
    if (status != PEP_STATUS_OK) {
        return status;
    }

    // Do engine
    status = gu_engine->adapter_group_invite_member(session, group_identity, group_member);
    if (status != PEP_STATUS_OK) {
        // Rollback
        PEP_STATUS rb_stat = gu_listman->adapter_group_remove_member(session, group_member, group_member);
        if (rb_stat != PEP_STATUS_OK) {
            //FATAL ERROR ON ROLLBACK
            status = (PEP_STATUS)-9999;
        }
    }
    return status;
}

PEP_STATUS adapter_group_remove_member(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *group_member)
{
    pEpLog("called");
    // Do listmanager
    PEP_STATUS status = gu_listman->adapter_group_remove_member(session, group_identity, group_member);
    if (status != PEP_STATUS_OK) {
        return status;
    }

    // Do engine
    status = gu_engine->adapter_group_remove_member(session, group_identity, group_member);
    if (status != PEP_STATUS_OK) {
        // Rollback
        PEP_STATUS rb_stat = gu_listman->adapter_group_invite_member(
            session,
            group_identity,
            group_member);
        if (rb_stat != PEP_STATUS_OK) {
            //FATAL ERROR ON ROLLBACK
            status = (PEP_STATUS)-9999;
        }
    }
    return status;
}

/*************************************************************************************************
 * Group query functions
 *************************************************************************************************/
// Listmanager Driver
GroupQueryDriverDummy group_query_driver_dummy{lm_dummy_db_path};
GroupQueryInterface *gq_listman = &group_query_driver_dummy;

DYNAMIC_API PEP_STATUS group_query_groups(PEP_SESSION session, identity_list **groups)
{
    pEpLog("called");
    return gq_listman->group_query_groups(session, groups);
}

DYNAMIC_API PEP_STATUS
group_query_manager(PEP_SESSION session, const pEp_identity *const group, pEp_identity **manager)
{
    pEpLog("called");
    return gq_listman->group_query_manager(session, group, manager);
}

DYNAMIC_API PEP_STATUS
group_query_members(PEP_SESSION session, const pEp_identity *const group, identity_list **members)
{
    pEpLog("called");
    return gq_listman->group_query_members(session, group, members);
}

#ifdef __cplusplus
}
#endif
