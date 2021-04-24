// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_GRP_UPDATE_DRV_ENGINE_HH
#define LIBPEPADAPTER_GRP_UPDATE_DRV_ENGINE_HH

#include "grp_update_interface.hh"
#include "pEpLog.hh"
#include <pEp/message_api.h>
#include <pEp/group.h>

namespace pEp {
    class GroupUpdateDriverEngine : public GroupUpdateInterface {
    public:
        PEP_STATUS adapter_group_create(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager,
            identity_list *memberlist) override;

        PEP_STATUS adapter_group_join(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *as_member) override;

        PEP_STATUS adapter_group_dissolve(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager) override;

        PEP_STATUS adapter_group_invite_member(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *group_member) override;

        PEP_STATUS adapter_group_remove_member(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *group_member) override;

        // Logging
        static bool log_enabled;
        Adapter::pEpLog::pEpLogger logger{ "GroupUpdateDriverEngine", log_enabled };

    private:
        Adapter::pEpLog::pEpLogger &m4gic_logger_n4ame = logger;
    };
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_UPDATE_DRV_ENGINE_HH
