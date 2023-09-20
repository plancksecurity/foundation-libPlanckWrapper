#include "grp_driver_engine.hh"
#include <pEp/pEpLog.hh>
#include <pEp/message_api.h>
#include <pEp/group.h>

using namespace std;

namespace pEp {
    namespace Adapter {
        bool GroupDriverEngine::log_enabled = false;

        GroupDriverEngine::GroupDriverEngine()
        {
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

        PEP_STATUS GroupDriverEngine::adapter_group_join(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *as_member,
            ::pEp_identity* manager
        ) noexcept
        {
            pEpLogClass("called");
            return ::group_join(session, group_identity, as_member);
        }

        // GroupQueryInterface
        PEP_STATUS GroupDriverEngine::group_query_groups(::PEP_SESSION session,
                                                         ::pEp_identity *manager,
                                                         ::identity_list **groups) noexcept
        {
            pEpLogClass("called");
            return ::retrieve_all_groups_as_manager(session, manager, groups);
        }

        PEP_STATUS GroupDriverEngine::group_query_manager(
                PEP_SESSION session,
                pEp_identity *group,
                pEp_identity **manager) noexcept
        {
            pEpLogClass("called");
            return ::get_group_manager(session, group, manager);
        }

        PEP_STATUS GroupDriverEngine::group_query_members(
                PEP_SESSION session,
                pEp_identity *group,
                identity_list **members) noexcept
        {
            pEpLogClass("called");
            return ::retrieve_active_member_ident_list(session, group, members);
        }
    } // namespace Adapter
} // namespace pEp