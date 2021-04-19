#include "ListManagerDummy.hh"
//#include "status_to_string.hh"

#include "pEpLog.hh"

namespace pEp {

    void ListManagerDummy::create(
        pEp_identity *group_identity,
        pEp_identity *manager,
        identity_list *memberlist)
    {
        pEpLog("called");
    }

    void ListManagerDummy::dissolve(pEp_identity *group_identity)
    {
        pEpLog("called");
    }

    void ListManagerDummy::join(pEp_identity *group_identity, pEp_identity *as_member)
    {
        pEpLog("called");
    }

    void ListManagerDummy::remove_member(pEp_identity *group_identity, pEp_identity *group_member)
    {
        pEpLog("called");
    }

    identity_list *ListManagerDummy::groups()
    {
        pEpLog("called");
        return NULL;
    }

    pEp_identity *ListManagerDummy::manager(const pEp_identity *const group)
    {
        pEpLog("called");
        return NULL;
    }

    identity_list *ListManagerDummy::members(const pEp_identity *const group)
    {
        pEpLog("called");
        return NULL;
    }
} // namespace pEp
