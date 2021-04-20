// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_GRP_UPDATE_DRV_DUMMY_HH
#define LIBPEPADAPTER_GRP_UPDATE_DRV_DUMMY_HH

#include "grp_update_interface.hh"
#include "pEpLog.hh"
#include "listmanager_dummy.hh"
#include <pEp/message_api.h>

namespace pEp {
    class GroupUpdateDriverDummy : public GroupUpdateInterface {
    public:
        GroupUpdateDriverDummy() = delete;
        GroupUpdateDriverDummy(const std::string& db_path);

        PEP_STATUS adapter_group_create(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager,
            identity_list *memberlist,
            pEp_group **group) override;

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
        Adapter::pEpLog::pEpLogger logger{ "GroupUpdateDriverDummy", log_enabled };

    private:
        ListManagerDummy lmd;
        Adapter::pEpLog::pEpLogger &m4gic_logger_n4ame = logger;
    };
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_UPDATE_DRV_DUMMY_HH
