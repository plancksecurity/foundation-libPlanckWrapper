// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_GRP_UPDATE_INTERFACE_HH
#define LIBPEPADAPTER_GRP_UPDATE_INTERFACE_HH

#include <pEp/message_api.h>
#include <pEp/group.h>

namespace pEp {
    class GroupUpdateInterface {
    public:
        virtual PEP_STATUS adapter_group_create(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager,
            identity_list *memberlist) = 0;

        virtual PEP_STATUS adapter_group_join(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *as_member) = 0;

        virtual PEP_STATUS adapter_group_dissolve(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager) = 0;

        virtual PEP_STATUS adapter_group_invite_member(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *group_member) = 0;

        virtual PEP_STATUS adapter_group_remove_member(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *group_member) = 0;

    private:
    };
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_UPDATE_INTERFACE_HH
