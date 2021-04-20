#include "grp_update_drv_engine.hh"
#include "pEpLog.hh"
#include <pEp/message_api.h>
#include <pEp/group.h>

using namespace std;

namespace pEp {
    bool GroupUpdateDriverEngine::log_enabled = false;

    PEP_STATUS GroupUpdateDriverEngine::adapter_group_create(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager,
        identity_list *memberlist,
        pEp_group **group)
    {
        pEpLogClass("called");
        return ::group_create(session, group_identity, manager, memberlist, group);
    }

    PEP_STATUS GroupUpdateDriverEngine::adapter_group_join(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *as_member)
    {
        pEpLogClass("called");
        return ::group_join(session, group_identity, as_member);
    }

    PEP_STATUS GroupUpdateDriverEngine::adapter_group_dissolve(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager)
    {
        pEpLogClass("called");
        return ::group_dissolve(session, group_identity, manager);
    }

    PEP_STATUS GroupUpdateDriverEngine::adapter_group_invite_member(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *group_member)
    {
        pEpLogClass("called");
        return ::group_invite_member(session, group_identity, group_member);
    }

    PEP_STATUS GroupUpdateDriverEngine::adapter_group_remove_member(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *group_member)
    {
        pEpLogClass("called");
        return ::group_remove_member(session, group_identity, group_member);
    }

} // namespace pEp