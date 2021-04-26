#include "grp_driver_engine.hh"
#include "pEpLog.hh"
#include <pEp/message_api.h>
#include <pEp/group.h>

using namespace std;

namespace pEp {
    namespace Adapter {
        bool GroupDriverEngine::log_enabled = false;

        GroupDriverEngine::GroupDriverEngine() {
            pEpLogClass("called");
        }

        PEP_STATUS GroupDriverEngine::adapter_group_create(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager,
            identity_list *memberlist) noexcept
        {
            pEpLogClass("called");
            return ::group_create(session, group_identity, manager, memberlist, nullptr);
        }

        PEP_STATUS GroupDriverEngine::adapter_group_dissolve(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager) noexcept
        {
            pEpLogClass("called");
            return ::group_dissolve(session, group_identity, manager);
        }

        PEP_STATUS GroupDriverEngine::adapter_group_invite_member(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *group_member) noexcept
        {
            pEpLogClass("called");
            return ::group_invite_member(session, group_identity, group_member);
        }

        PEP_STATUS GroupDriverEngine::adapter_group_remove_member(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *group_member) noexcept
        {
            pEpLogClass("called");
            return ::group_remove_member(session, group_identity, group_member);
        }
    } // namespace Adapter
} // namespace pEp