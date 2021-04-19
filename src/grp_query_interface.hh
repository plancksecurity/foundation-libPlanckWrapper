// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_GRP_QUERY_INTERFACE_HH
#define LIBPEPADAPTER_GRP_QUERY_INTERFACE_HH

#include <pEp/message_api.h>
#include <pEp/group.h>

namespace pEp {
    class GroupQueryInterface {
    public:
        virtual PEP_STATUS group_query_groups(PEP_SESSION session, identity_list **groups) = 0;

        virtual PEP_STATUS group_query_manager(
            PEP_SESSION session,
            const pEp_identity *const group,
            pEp_identity **manager) = 0;

        virtual PEP_STATUS group_query_members(
            PEP_SESSION session,
            const pEp_identity *const group,
            identity_list **members) = 0;

    private:
    };
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_QUERY_INTERFACE_HH
