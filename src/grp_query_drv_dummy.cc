#include "grp_query_drv_dummy.hh"
#include "utils.hh"
#include "pEpLog.hh"
#include "listmanager_dummy.hh"
#include <pEp/message_api.h>
#include <pEp/group.h>

using namespace std;

namespace pEp {
    bool GroupQueryDriverDummy::log_enabled = false;

    GroupQueryDriverDummy::GroupQueryDriverDummy(const std::string &db_path) :
        lmd(ListManagerDummy(db_path))
    {
        pEpLogClass("called");
    }

    PEP_STATUS GroupQueryDriverDummy::group_query_groups(PEP_SESSION session, identity_list **groups) noexcept
    {
        pEpLogClass("called");
        PEP_STATUS status = PEP_UNKNOWN_ERROR;

        if (!session) {
            status = PEP_ILLEGAL_VALUE;
        } else {
            vector<string> lists_queried;
            try {
                lists_queried = lmd.lists();
                status = PEP_STATUS_OK;
            } catch (const exception &e) {
                pEpLogClass(Utils::nested_exception_to_string(e));
                status = PEP_UNKNOWN_ERROR;
            } catch (...) {
                pEpLogClass("unknown exception");
                status = PEP_UNKNOWN_ERROR;
            }

            if (status == PEP_STATUS_OK) {
                ::identity_list *idl_groups = ::new_identity_list(nullptr);
                for (const string &addr_list : lists_queried) {
                    ::pEp_identity *grp_ident = ::new_identity(
                        addr_list.c_str(),
                        nullptr,
                        nullptr,
                        nullptr);
                    ::update_identity(session, grp_ident);
                    identity_list_add(idl_groups, grp_ident);
                }
                *groups = idl_groups;
            }
        }
        return status;
    }

    PEP_STATUS GroupQueryDriverDummy::group_query_manager(
        PEP_SESSION session,
        const pEp_identity *const group,
        pEp_identity **manager) noexcept
    {
        pEpLogClass("called");
        PEP_STATUS status = PEP_UNKNOWN_ERROR;

        if (!session || !group) {
            status = PEP_ILLEGAL_VALUE;
        } else {
            const string addr_list{ group->address };
            string addr_manager{};
            try {
                addr_manager = lmd.moderator(addr_list);
                status = PEP_STATUS_OK;
            } catch (const ListDoesNotExistException &e) {
                pEpLogClass(Utils::nested_exception_to_string(e));
                status = PEP_GROUP_NOT_FOUND;
            } catch (const exception &e) {
                pEpLogClass(Utils::nested_exception_to_string(e));
                status = PEP_UNKNOWN_ERROR;
            } catch (...) {
                pEpLogClass("unknown exception");
                status = PEP_UNKNOWN_ERROR;
            }

            if (status == PEP_STATUS_OK) {
                ::pEp_identity *manager_queried = ::new_identity(
                    addr_manager.c_str(),
                    nullptr,
                    nullptr,
                    nullptr);
                ::update_identity(session, manager_queried);
                *manager = manager_queried;
            }
        }
        return status;
    }

    PEP_STATUS GroupQueryDriverDummy::group_query_members(
        PEP_SESSION session,
        const pEp_identity *const group,
        identity_list **members) noexcept
    {
        pEpLogClass("called");
        PEP_STATUS status = PEP_UNKNOWN_ERROR;

        if (!session || !group) {
            status = PEP_ILLEGAL_VALUE;
        } else {
            if (!Utils::is_c_str_empty(group->address)) {
                status = PEP_ILLEGAL_VALUE;
            } else {
                const string addr_grp{ group->address };
                vector<string> members_queried;
                try {
                    members_queried = lmd.members(addr_grp);
                    status = PEP_STATUS_OK;
                } catch (const ListDoesNotExistException &e) {
                    pEpLogClass(Utils::nested_exception_to_string(e));
                    status = PEP_GROUP_NOT_FOUND;
                } catch (const exception &e) {
                    pEpLogClass(Utils::nested_exception_to_string(e));
                    status = PEP_UNKNOWN_ERROR;
                } catch (...) {
                    pEpLogClass("unknown exception");
                    status = PEP_UNKNOWN_ERROR;
                }

                if (status == PEP_STATUS_OK) {
                    ::identity_list *idl_members = ::new_identity_list(nullptr);
                    for (const string &addr_member : members_queried) {
                        ::pEp_identity *member_ident = ::new_identity(
                            addr_member.c_str(),
                            nullptr,
                            nullptr,
                            nullptr);
                        ::update_identity(session, member_ident);
                        identity_list_add(idl_members, member_ident);
                    }
                    *members = idl_members;
                }
            }
        }
        return status;
    }
} // namespace pEp