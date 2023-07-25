// This file is under GNU General Public License 3.0
// see LICENSE.txt
// clang-format off
#ifndef LIBPEPADAPTER_GROUP_MANAGER_API_H
#define LIBPEPADAPTER_GROUP_MANAGER_API_H

#include <pEp/message_api.h>

#ifdef __cplusplus
extern "C" {
#endif

DYNAMIC_API PEP_STATUS adapter_group_init();

/*************************************************************************************************
 * Group management functions
 *************************************************************************************************/

/**
 *  <!--       adapter_group_create()       -->
 *
 *  @brief      Create a group in the database with the input group_identity and manager and invite new members to the group
 *              if this is an own group (for the external API, this is always the case).
 *
 *              This function sets up the actual database structures for a group and invites new members to the group.
 *
 *              For the external API, it is used when creating an own group. The group is represented by the
 *              incoming group_identity, which contains the user_id and address for the group.
 *              If no key is present for the former, it will be generated - if there is already
 *              a default key for the group_identity in the database, that will be used instead.
 *              The manager
 *
 *  @param[in]      session             associated session object
 *  @param[in,out]  group_identity      the pEp_identity object representing the group. Must contain at least
 *                                      a user_id and address
 *  @param[in,out]  manager             the pEp_identity object representing the group's manager. Must contain
 *                                      a user_id and address, and there must be a default key for the manager
 *                                      present in the database
 *  @param[in,out]  member_ident_list   list of group member identities
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      All input values stay with the caller
 *
 *  @warning        starts a DB transaction - do not call from within a function which
 *                  is already in the middle of another one.
 *
 *  @note           in,out fields are labelled as such because they get updated by update_identity()/myself()
 *                  and have group flags added. group_identity may have its user_id freed and replaced
 *                  with the canonical own user id.
 *
 */
DYNAMIC_API PEP_STATUS adapter_group_create(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *manager,
    identity_list *memberlist);


/**
 *  <!--       adapter_group_dissolve()       -->
 *
 *  @brief          Dissolve a group, revoke its key, notify all members of the dissolution and
 *                  revocation, and mark the group as inactive in the database
 *
 *  @param[in]      session             associated session object
 *  @param[in]      group_identity      the pEp_identity object representing the group. Must contain at least
 *                                      a user_id and address
 *  @param[in]      manager             the pEp_identity object representing the group's manager. Must contain
 *                                      a user_id and address, and there must be a default key for the manager
 *                                      present in the database
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      FIXME
 *
 *  @warning        For recipients to accept the dissolution, the sender/manager key used must be a key that they
 *                  have a trust entry for.
 */
DYNAMIC_API PEP_STATUS adapter_group_dissolve(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *manager);

/**
 *  <!--       adapter_group_invite_member()       -->
 *
 *  @brief      Invite a member to an extant group, marking the member as invited in the database and
 *              sending out an invitation to said member
 *
 *  @param[in]      session             associated session object
 *  @param[in]      group_identity      the pEp_identity object representing the group. Must contain at least
 *                                      a user_id and address
 *  @param[in]      group_member        the pEp_identity object representing the member to invite. Must contain
 *                                      a user_id and address, and there must be a default key for the member
 *                                      present in the database
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      FIXME
 *
 *  @note           This generates a GroupCreate message even though the group already exists - this is because
 *                  this is the accepted message format for invitations to potential members
 *
 */
DYNAMIC_API PEP_STATUS adapter_group_invite_member(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *group_member);

/**
 *  <!--       adapter_group_remove_member()       -->
 *
 *  @brief      Remove a member from a group, deleting the member from the member list and executing a key
 *              reset on the group identity
 *
 *  @param[in]      session             associated session object
 *  @param[in]      group_identity      the pEp_identity object representing the group. Must contain at least
 *                                      a user_id and address
 *  @param[in]      group_member        the pEp_identity object representing the member to remove. Must contain
 *                                      a user_id and address
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      FIXME
 *
 *  @todo           Revamp implementation and execute key reset
 *
 */
DYNAMIC_API PEP_STATUS adapter_group_remove_member(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *group_member);


/**
 *  <!--       group_join()       -->
 *
 *  @brief          Join a group for which we have received an invitation, marking
 *                  our own membership in the database for the group and sending the manager
 *                  a confirmation of the acceptance of the invitation
 *
 *  @param[in]      session             associated session object
 *  @param[in]      group_identity      the pEp_identity object representing the group. Must contain at least
 *                                      a user_id and address
 *  @param[in]      as_member           the pEp_identity object representing the own identity we want to use to
 *                                      join the group. This must match the identity which was invited to the group.
 *                                      Must contain a user_id and address.
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      FIXME
 *
 *
 */
DYNAMIC_API PEP_STATUS adapter_group_join(
    PEP_SESSION session,
    pEp_identity *group_identity,
    pEp_identity *as_member,
    pEp_identity* manager = nullptr
);

/*************************************************************************************************
 * Group query functions
 *************************************************************************************************/

/**
 *  <!--       adapter_group_query_groups()       -->
 *
 *  @brief          queries the list manager which groups currently exist.
 *
 *  @param[in]      session             associated session object
 *  @param[out]     groups              list of pEp_identity representing
 *                                      all group identities that currently exist.
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      ownership of all parameters goes to the caller
 *
 */

DYNAMIC_API PEP_STATUS adapter_group_query_groups(
    PEP_SESSION session,
    identity_list **groups);

/**
 *  <!--       adapter_group_query_manager()       -->
 *
 *  @brief          queries the list manager for the group manager of a given group.
 *
 *  @param[in]      session             associated session object
 *  @param[in]      group               pEp_Identity representing the group identity in question
 *  @param[out]     manager             pEp_identity representing the group manager for "group"
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      ownership of all parameters goes to the caller
 *
 */

DYNAMIC_API PEP_STATUS adapter_group_query_manager(
    PEP_SESSION session,
    const pEp_identity *group,
    pEp_identity **manager);

/**
 *  <!--       adapter_group_query_members()       -->
 *
 *  @brief          queries the list manager for all members of a given group.
 *
 *  @param[in]      session             associated session object
 *  @param[in]      group               pEp_Identity representing the group identity in question
 *  @param[out]     members             list of pEp_identity representing all the members of "group"
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      ownership of all parameters goes to the caller
 *
 */

DYNAMIC_API PEP_STATUS adapter_group_query_members(
    PEP_SESSION session,
    const pEp_identity *group,
    identity_list **members);


#ifdef __cplusplus
}
#endif

#endif //LIBPEPADAPTER_GROUP_MANAGER_API_H