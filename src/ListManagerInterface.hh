// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_LISTMANAGERINTERFACE_HH
#define LIBPEPADAPTER_LISTMANAGERINTERFACE_HH

#include <pEp/message_api.h>

namespace pEp {
    class ListManagerInterface {
    public:
        // update functions
        // Group create/destroy
        virtual void create(
            pEp_identity *group_identity,
            pEp_identity *manager,
            identity_list *memberlist) = 0;
        virtual void dissolve(pEp_identity *group_identity) = 0;

        // Members create/destroy
        virtual void join(pEp_identity *group_identity, pEp_identity *as_member) = 0;
        virtual void remove_member(pEp_identity *group_identity, pEp_identity *group_member) = 0;

        // query functions
        virtual identity_list *groups() = 0;
        virtual pEp_identity *manager(const pEp_identity *const group) = 0;
        virtual identity_list *members(const pEp_identity *const group) = 0;

    private:
    };
} // namespace pEp

#endif // LIBPEPADAPTER_LISTMANAGERINTERFACE_HH
