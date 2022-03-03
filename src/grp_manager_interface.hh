// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_GRP_MANAGER_INTERFACE_HH
#define LIBPEPADAPTER_GRP_MANAGER_INTERFACE_HH

#include <pEp/message_api.h>

namespace pEp {
    namespace Adapter {
        class GroupUpdateInterface {
        public:
            virtual PEP_STATUS adapter_group_create(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *manager,
                ::identity_list *memberlist) noexcept = 0;

            virtual PEP_STATUS adapter_group_dissolve(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *manager) noexcept = 0;

            virtual PEP_STATUS adapter_group_invite_member(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *group_member) noexcept = 0;

            virtual PEP_STATUS adapter_group_remove_member(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *group_member) noexcept = 0;

            virtual PEP_STATUS adapter_group_join(
                ::PEP_SESSION session,
                ::pEp_identity *group_identity,
                ::pEp_identity *as_member)  noexcept = 0;

        private:
        };

        class GroupQueryInterface {
        public:
            virtual PEP_STATUS group_query_groups(
                ::PEP_SESSION session,
                ::identity_list **groups) noexcept = 0;

            virtual PEP_STATUS group_query_manager(
                ::PEP_SESSION session,
                const ::pEp_identity *group,
                ::pEp_identity **manager) noexcept = 0;

            virtual PEP_STATUS group_query_members(
                ::PEP_SESSION session,
                const ::pEp_identity *group,
                ::identity_list **members) noexcept = 0;

        private:
        };

        class GroupManagerInterface : public GroupUpdateInterface, public GroupQueryInterface {
        };
    } // namespace Adapter
} // namespace pEp

#endif // LIBPEPADAPTER_GRP_MANAGER_INTERFACE_HH
