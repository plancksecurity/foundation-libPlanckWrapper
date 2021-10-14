// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_GRP_DRIVER_REPLICATOR_HH
#define LIBPEPADAPTER_GRP_DRIVER_REPLICATOR_HH

#include <memory>
#include "grp_manager_interface.hh"
#include "grp_driver_engine.hh"
#include "grp_driver_dummy.hh"
#include "pEpLog.hh"
#include <pEp/message_api.h>

namespace pEp {
    namespace Adapter {
        class GroupDriverReplicator : public GroupManagerInterface {
        public:
            GroupDriverReplicator();

            void set_replication_source(GroupManagerInterface &src);
            void set_replication_destination(GroupUpdateInterface &dst);

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

            PEP_STATUS adapter_group_join(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *as_member)  noexcept override;

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
            Adapter::pEpLog::pEpLogger logger{ "GroupDriverReplicator", log_enabled };

        private:
            // Group replication roles
            // Default replication source and destination
            std::shared_ptr<GroupManagerInterface> default_repl_src;
            std::shared_ptr<GroupUpdateInterface> default_repl_dst;

            // Current replication source and destination
            GroupManagerInterface *repl_src = nullptr; // Source needs full interface
            GroupUpdateInterface *repl_dst = nullptr;  // Destination needs update interface only

            // Helpers
            bool has_repl_src_and_dst();
            // Logging
            Adapter::pEpLog::pEpLogger &m4gic_logger_n4me = logger;
        };
    } // namespace Adapter
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_DRIVER_REPLICATOR_HH
