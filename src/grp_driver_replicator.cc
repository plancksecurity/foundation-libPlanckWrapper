// This file is under GNU General Public License 3.0
// see LICENSE.txt
#include "grp_driver_replicator.hh"

using namespace std;


namespace pEp {
    namespace Adapter {
        bool GroupDriverReplicator::log_enabled = false;

        GroupDriverReplicator::GroupDriverReplicator()
        {
            pEpLogClass("called");
            const string lm_dummy_db_filename = "listman_dummy.db";
#ifdef WIN32
            const string lm_dummy_db_path = string(::per_user_directory()) + "\\" +
                                            lm_dummy_db_filename;
#else
            const string lm_dummy_db_path = string(::per_user_directory()) + "/" +
                                            lm_dummy_db_filename;
#endif
            default_repl_src = make_shared<GroupDriverDummy>(lm_dummy_db_path);
            set_replication_source(*default_repl_src);

            default_repl_dst = make_shared<GroupDriverEngine>();
            set_replication_destination(*default_repl_dst);
        }

        void GroupDriverReplicator::set_replication_source(GroupManagerInterface &src)
        {
            pEpLogClass("called");
            repl_src = &src;
        }

        void GroupDriverReplicator::set_replication_destination(GroupUpdateInterface &dst)
        {
            pEpLogClass("called");
            repl_dst = &dst;
        }

        // GroupUpdateInterface
        PEP_STATUS GroupDriverReplicator::adapter_group_create(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager,
            identity_list *memberlist) noexcept
        {
            pEpLogClass("called");
            if (!has_repl_src_and_dst()) {
                return PEP_UNKNOWN_ERROR;
            }

            // Do listmanager
            PEP_STATUS status = repl_src->adapter_group_create(
                session,
                group_identity,
                manager,
                memberlist);
            if (status != PEP_STATUS_OK) {
                return status;
            }

            // Do engine
            status = repl_dst->adapter_group_create(session, group_identity, manager, memberlist);
            if (status != PEP_STATUS_OK) {
                // Rollback listman
                PEP_STATUS rb_stat = repl_src->adapter_group_dissolve(session, group_identity, manager);
                if (rb_stat != PEP_STATUS_OK) {
                    //FATAL ERROR ON ROLLBACK
                    status = (PEP_STATUS)-9999;
                }
            }
            return status;
        }


        PEP_STATUS GroupDriverReplicator::adapter_group_dissolve(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *manager) noexcept
        {
            pEpLogClass("called");
            if (!has_repl_src_and_dst()) {
                return PEP_UNKNOWN_ERROR;
            }
            // Do listmanager
            PEP_STATUS status = repl_src->adapter_group_dissolve(session, group_identity, manager);
            if (status != PEP_STATUS_OK) {
                return status;
            }

            // Do engine
            status = repl_dst->adapter_group_dissolve(session, group_identity, manager);
            if (status != PEP_STATUS_OK) {
                // Rollback listman
                // TODO: ????
                //        PEP_STATUS rb_stat = gu_listman->adapter_group_dissolve(session, group_identity, manager);
                //        if (rb_stat != PEP_STATUS_OK) {
                //            //FATAL ERROR ON ROLLBACK
                //            status = (PEP_STATUS)-9999;
                //        }
            }
            return status;
        }

        PEP_STATUS GroupDriverReplicator::adapter_group_invite_member(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *group_member) noexcept
        {
            pEpLogClass("called");
            if (!has_repl_src_and_dst()) {
                return PEP_UNKNOWN_ERROR;
            }

            // Do listmanager
            PEP_STATUS status = repl_src->adapter_group_invite_member(
                session,
                group_identity,
                group_member);
            if (status != PEP_STATUS_OK) {
                return status;
            }

            // Do engine
            status = repl_dst->adapter_group_invite_member(session, group_identity, group_member);
            if (status != PEP_STATUS_OK) {
                // Rollback
                PEP_STATUS rb_stat = repl_src->adapter_group_remove_member(
                    session,
                    group_member,
                    group_member);
                if (rb_stat != PEP_STATUS_OK) {
                    //FATAL ERROR ON ROLLBACK
                    status = (PEP_STATUS)-9999;
                }
            }
            return status;
        }

        PEP_STATUS GroupDriverReplicator::adapter_group_remove_member(
            PEP_SESSION session,
            pEp_identity *group_identity,
            pEp_identity *group_member) noexcept
        {
            pEpLogClass("called");
            if (!has_repl_src_and_dst()) {
                return PEP_UNKNOWN_ERROR;
            }

            // Do listmanager
            PEP_STATUS status = repl_src->adapter_group_remove_member(
                session,
                group_identity,
                group_member);
            if (status != PEP_STATUS_OK) {
                return status;
            }

            // Do engine
            status = repl_dst->adapter_group_remove_member(session, group_identity, group_member);
            if (status != PEP_STATUS_OK) {
                // Rollback
                PEP_STATUS rb_stat = repl_src->adapter_group_invite_member(
                    session,
                    group_identity,
                    group_member);
                if (rb_stat != PEP_STATUS_OK) {
                    //FATAL ERROR ON ROLLBACK
                    status = (PEP_STATUS)-9999;
                }
            }
            return status;
        }

        // GroupQueryInterface
        PEP_STATUS GroupDriverReplicator::group_query_groups(
            PEP_SESSION session,
            identity_list **groups) noexcept
        {
            pEpLogClass("called");
            if (!has_repl_src_and_dst()) {
                return PEP_UNKNOWN_ERROR;
            }

            return repl_src->group_query_groups(session, groups);
        }

        PEP_STATUS GroupDriverReplicator::group_query_manager(
            PEP_SESSION session,
            const pEp_identity *const group,
            pEp_identity **manager) noexcept
        {
            pEpLogClass("called");
            if (!has_repl_src_and_dst()) {
                return PEP_UNKNOWN_ERROR;
            }

            return repl_src->group_query_manager(session, group, manager);
        }

        PEP_STATUS GroupDriverReplicator::group_query_members(
            PEP_SESSION session,
            const pEp_identity *const group,
            identity_list **members) noexcept
        {
            pEpLogClass("called");
            if (!has_repl_src_and_dst()) {
                return PEP_UNKNOWN_ERROR;
            }

            return repl_src->group_query_members(session, group, members);
        }

        bool GroupDriverReplicator::has_repl_src_and_dst()
        {
            bool ret = true;
            if (!repl_src) {
                ret = false;
                pEpLogClass("Abort: no replication source (listmanager)");
            }
            if (!repl_dst) {
                ret = false;
                pEpLogClass("Abort: no replication destination (pEpEngine)");
            }
            return ret;
        }
    } // namespace Adapter
} // namespace pEp