// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_GRP_DRIVER_DUMMY_HH
#define LIBPEPADAPTER_GRP_DRIVER_DUMMY_HH

#include "grp_manager_interface.hh"
#include "pEpLog.hh"
#include "listmanager_dummy.hh"
#include <pEp/message_api.h>

namespace pEp {
    namespace Adapter {
        class GroupDriverDummy : public GroupManagerInterface {
        public:
            GroupDriverDummy() = delete;
            GroupDriverDummy(const std::string &db_path);

            // GroupUpdateInterface
            PEP_STATUS adapter_group_create(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *manager,
                ::identity_list *memberlist) noexcept override;

            PEP_STATUS adapter_group_dissolve(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *manager) noexcept override;

            PEP_STATUS adapter_group_invite_member(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *group_member) noexcept override;

            PEP_STATUS adapter_group_remove_member(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *group_member) noexcept override;

            // GroupQueryInterface
            PEP_STATUS group_query_groups(::PEP_SESSION session, ::identity_list **groups) noexcept override;

            PEP_STATUS group_query_manager(
                ::PEP_SESSION session,
                const ::pEp_identity *const group,
                ::pEp_identity **manager) noexcept override;

            PEP_STATUS group_query_members(
                ::PEP_SESSION session,
                const ::pEp_identity *const group,
                ::identity_list **members) noexcept override;

            // Logging
            static bool log_enabled;
            Adapter::pEpLog::pEpLogger logger{ "GroupDriverDummy", log_enabled };

        private:
            ListManagerDummy lmd;
            Adapter::pEpLog::pEpLogger &m4gic_logger_n4me = logger;
        };
    } // namespace Adapter
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_DRIVER_DUMMY_HH
