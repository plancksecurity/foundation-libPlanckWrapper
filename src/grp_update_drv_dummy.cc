#include "grp_update_drv_dummy.hh"
#include "pEpLog.hh"
#include <pEp/message_api.h>
#include "listmanager_dummy.hh"

using namespace std;

namespace pEp {
    bool GroupUpdateDriverDummy::log_enabled = false;

    GroupUpdateDriverDummy::GroupUpdateDriverDummy(const std::string &db_path) :
        lmd(ListManagerDummy(db_path))
    {
        pEpLogClass("called");
    }

    PEP_STATUS GroupUpdateDriverDummy::adapter_group_create(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager,
        identity_list *memberlist) noexcept
    {
        pEpLogClass("called");
        string addr_list = group_identity->address;
        string addr_manager = manager->address;
        lmd.list_add(addr_list, addr_manager);
        // TODO:ADD MEMBERLIST

        return PEP_STATUS_OK;
    }

    //    PEP_STATUS GroupUpdateDriverDummy::adapter_group_join(
    //        PEP_SESSION session,
    //        pEp_identity *group_identity,
    //        pEp_identity *as_member) noexcept
    //    {
    //        pEpLogClass("called");
    //
    //        return PEP_STATUS_OK;
    //    }

    PEP_STATUS GroupUpdateDriverDummy::adapter_group_dissolve(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager) noexcept
    {
        pEpLogClass("called");

        return PEP_STATUS_OK;
    }

    PEP_STATUS GroupUpdateDriverDummy::adapter_group_invite_member(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *group_member) noexcept
    {
        pEpLogClass("called");
        return PEP_STATUS_OK;
    }

    PEP_STATUS GroupUpdateDriverDummy::adapter_group_remove_member(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *group_member) noexcept
    {
        pEpLogClass("called");
        return PEP_STATUS_OK;
    }

} // namespace pEp