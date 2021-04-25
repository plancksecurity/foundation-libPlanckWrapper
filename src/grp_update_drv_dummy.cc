#include "grp_update_drv_dummy.hh"
#include "pEpLog.hh"
#include "utils.hh"
#include <pEp/message_api.h>
#include "listmanager_dummy.hh"

using namespace std;

namespace pEp {
    bool GroupUpdateDriverDummy::log_enabled = false;

    GroupUpdateDriverDummy::GroupUpdateDriverDummy(const std::string &db_path) :
        lmd(ListManagerDummy(db_path))
    {
        pEpLogClass("called");
    }

    PEP_STATUS GroupUpdateDriverDummy::adapter_group_create(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager,
        identity_list *memberlist) noexcept
    {
        pEpLogClass("called");
        PEP_STATUS status = PEP_UNKNOWN_ERROR;

        if (!group_identity || !manager) {
            status = PEP_ILLEGAL_VALUE;
        } else {
            if (Utils::is_c_str_empty(group_identity->address) ||
                Utils::is_c_str_empty(manager->address)) {
                status = PEP_ILLEGAL_VALUE;
            } else {
                const string addr_list{ group_identity->address };
                const string addr_manager{ manager->address };

                try {
                    lmd.list_add(addr_list, addr_manager);
                    status = PEP_STATUS_OK;
                } catch (const AlreadyExistsException &e) {
                    pEpLogClass(Utils::nested_exception_to_string(e));
                    status = PEP_GROUP_EXISTS;
                } catch (const exception &e) {
                    pEpLogClass(Utils::nested_exception_to_string(e));
                    status = PEP_UNKNOWN_ERROR;
                } catch (...) {
                    pEpLogClass("unknown exception");
                    status = PEP_UNKNOWN_ERROR;
                }

                if (status == PEP_STATUS_OK) {
                    // Add the memberlist (if given)
                    // Fail totally on the first member_invite() that fails
                    const vector<pEp_identity *> cxx_memberlist = Utils::to_cxx(*memberlist);
                    for (pEp_identity *const member : cxx_memberlist) {
                        status = this->adapter_group_invite_member(session, group_identity, member);
                        if (status != PEP_STATUS_OK) {
                            status = status;
                            break;
                        }
                    }
                }
            }
        }
        return status;
    }

    //    PEP_STATUS GroupUpdateDriverDummy::adapter_group_join(
    //        PEP_SESSION session,
    //        pEp_identity *group_identity,
    //        pEp_identity *as_member) noexcept
    //    {
    //        pEpLogClass("called");
    //
    //        return PEP_STATUS_OK;
    //    }

    // The engine checks if the manager is correct for the group given
    // But the list manager does not require that
    // So, we verify that first, too. using moderator()
    PEP_STATUS GroupUpdateDriverDummy::adapter_group_dissolve(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager) noexcept
    {
        pEpLogClass("called");
        PEP_STATUS status = PEP_UNKNOWN_ERROR;

        if (!group_identity || !manager) {
            status = PEP_ILLEGAL_VALUE;
        } else {
            if (Utils::is_c_str_empty(group_identity->address) ||
                Utils::is_c_str_empty(manager->address)) {
                status = PEP_ILLEGAL_VALUE;
            } else {
                const string addr_list{ group_identity->address };
                const string addr_manager{ manager->address };

                // Check if given manager is correct for the given group
                string addr_manager_queried;
                try {
                    addr_manager_queried = lmd.moderator(addr_list);
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
                    if (addr_manager_queried != addr_manager) {
                        status = PEP_CANNOT_DISABLE_GROUP;
                    } else {
                        try {
                            lmd.list_delete(addr_list);
                            status = PEP_STATUS_OK;
                        } catch (const MemberDoesNotExistException &e) {
                            pEpLogClass(Utils::nested_exception_to_string(e));
                            // TODO: Silently succeed???
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
                    }
                }
            }
        }
        return status;
    }

    PEP_STATUS GroupUpdateDriverDummy::adapter_group_invite_member(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *group_member) noexcept
    {
        pEpLogClass("called");

        PEP_STATUS status = PEP_UNKNOWN_ERROR;

        if (!group_identity || !group_member) {
            status = PEP_ILLEGAL_VALUE;
        } else {
            if (Utils::is_c_str_empty(group_identity->address) ||
                Utils::is_c_str_empty(group_member->address)) {
                status = PEP_ILLEGAL_VALUE;
            } else {
                const string addr_list{ group_identity->address };
                const string addr_member{ group_member->address };

                try {
                    lmd.member_add(addr_list, addr_member);
                    status = PEP_STATUS_OK;
                } catch (const AlreadyExistsException &e) {
                    // TODO: Silently succeed???
                    pEpLogClass(Utils::nested_exception_to_string(e));
                    status = PEP_STATUS_OK;
                } catch (const ListDoesNotExistException &e) {
                    pEpLogClass(Utils::nested_exception_to_string(e));
                    return PEP_GROUP_NOT_FOUND;
                } catch (const exception &e) {
                    pEpLogClass(Utils::nested_exception_to_string(e));
                    status = PEP_UNKNOWN_ERROR;
                } catch (...) {
                    pEpLogClass("unknown exception");
                    status = PEP_UNKNOWN_ERROR;
                }
            }
        }
        return status;
    }

    PEP_STATUS GroupUpdateDriverDummy::adapter_group_remove_member(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *group_member) noexcept
    {
        pEpLogClass("called");
        PEP_STATUS status = PEP_UNKNOWN_ERROR;

        if (!group_identity || !group_member) {
            status = PEP_ILLEGAL_VALUE;
        } else {
            if (Utils::is_c_str_empty(group_identity->address) ||
                Utils::is_c_str_empty(group_member->address)) {
                status = PEP_ILLEGAL_VALUE;
            } else {
                const string addr_list{ group_identity->address };
                const string addr_member{ group_member->address };

                try {
                    lmd.member_remove(addr_list, addr_member);
                    status = PEP_STATUS_OK;
                } catch (const MemberDoesNotExistException &e) {
                    pEpLogClass(Utils::nested_exception_to_string(e));
                    // TODO: Silently succeed???
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
            }
        }
        return status;
    }

} // namespace pEp