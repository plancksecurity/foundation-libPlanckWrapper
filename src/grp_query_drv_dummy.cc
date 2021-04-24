#include "grp_query_drv_dummy.hh"
#include "pEpLog.hh"
#include "listmanager_dummy.hh"
#include <pEp/message_api.h>
#include <pEp/group.h>

using namespace std;

namespace pEp {
    bool GroupQueryDriverDummy::log_enabled = false;

    GroupQueryDriverDummy::GroupQueryDriverDummy(const std::string &db_path) :
        lmd(ListManagerDummy(db_path))
    {
        pEpLogClass("called");
    }

    PEP_STATUS GroupQueryDriverDummy::group_query_groups(PEP_SESSION session, identity_list **groups) noexcept
    {
        pEpLogClass("called");
        return PEP_STATUS_OK;
    }

    PEP_STATUS GroupQueryDriverDummy::group_query_manager(
        PEP_SESSION session,
        const pEp_identity *const group,
        pEp_identity **manager) noexcept
    {
        pEpLogClass("called");
        return PEP_STATUS_OK;
    }

    PEP_STATUS GroupQueryDriverDummy::group_query_members(
        PEP_SESSION session,
        const pEp_identity *const group,
        identity_list **members) noexcept
    {
        pEpLogClass("called");
        return PEP_STATUS_OK;
    }
} // namespace pEp