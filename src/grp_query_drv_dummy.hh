// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_GRP_QUERY_DRV_DUMMY_HH
#define LIBPEPADAPTER_GRP_QUERY_DRV_DUMMY_HH

#include "grp_query_interface.hh"
#include "pEpLog.hh"
#include "listmanager_dummy.hh"
#include <pEp/message_api.h>
#include <pEp/group.h>

namespace pEp {
    class GroupQueryDriverDummy : public GroupQueryInterface {
    public:
        GroupQueryDriverDummy() = delete;
        GroupQueryDriverDummy(const std::string &db_path);

        PEP_STATUS group_query_groups(PEP_SESSION session, identity_list **groups) override;
        PEP_STATUS group_query_manager(
            PEP_SESSION session,
            const pEp_identity *const group,
            pEp_identity **manager) override;
        PEP_STATUS group_query_members(
            PEP_SESSION session,
            const pEp_identity *const group,
            identity_list **members) override;

        // Logging
        static bool log_enabled;
        Adapter::pEpLog::pEpLogger logger{ "GroupQueryDriverDummy", log_enabled };

    private:
        ListManagerDummy lmd;
        Adapter::pEpLog::pEpLogger &m4gic_logger_n4ame = logger;
    };
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_QUERY_DRV_DUMMY_HH
