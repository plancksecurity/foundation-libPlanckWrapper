// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_LISTMANAGERDUMMY_HH
#define LIBPEPADAPTER_LISTMANAGERDUMMY_HH

#include "ListManagerInterface.hh"

namespace pEp {
    class ListManagerDummy : public ListManagerInterface {
    public:
        // update functions
        // Group create/destroy
        void create(pEp_identity *group_identity, pEp_identity *manager, identity_list *memberlist) override;
        void dissolve(pEp_identity *group_identity) override;

        // Members create/destroy
        void join(pEp_identity *group_identity, pEp_identity *as_member) override;
        void remove_member(pEp_identity *group_identity, pEp_identity *group_member) override;

        // query functions
        identity_list *groups() override;
        pEp_identity *manager(const pEp_identity *const group) override;
        identity_list *members(const pEp_identity *const group) override;

    private:
    };
} // namespace pEp

#endif // LIBPEPADAPTER_LISTMANAGERDUMMY_HH
