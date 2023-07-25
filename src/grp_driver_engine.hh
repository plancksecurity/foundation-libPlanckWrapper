// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_GRP_DRIVER_ENGINE_HH
#define LIBPEPADAPTER_GRP_DRIVER_ENGINE_HH

#include "grp_manager_interface.hh"
#include <pEp/pEpLog.hh>
#include <pEp/message_api.h>

namespace pEp {
    namespace Adapter {
        class GroupDriverEngine : public GroupUpdateInterface {
        public:
            GroupDriverEngine();

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
                ::pEp_identity *as_member,
                ::pEp_identity* manager = nullptr
            )  noexcept override;

            // Logging
            static bool log_enabled;
            Adapter::pEpLog::pEpLogger logger{ "GroupDriverEngine", log_enabled };

        private:
            Adapter::pEpLog::pEpLogger &m4gic_logger_n4me = logger;
        };
    } // namespace Adapter
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_DRIVER_ENGINE_HH
