// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_GROUP_HH
#define LIBPEPADAPTER_GROUP_HH


#ifdef __cplusplus
extern "C" {
#endif


/*************************************************************************************************
 * Group management functions
 *************************************************************************************************/

/**
 *  <!--       group_create()       -->
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
 *  @param[in]      group_identity      the pEp_identity object representing the group. Must contain at least
 *                                      a user_id and address
 *  @param[in]      manager             the pEp_identity object representing the group's manager. Must contain
 *                                      a user_id and address, and there must be a default key for the manager
 *                                      present in the database
 *  @param[in]      memberlist          list of group members
 *  @param[in,out]  group               Optional reference for pointer to group object
 *                                      representing the created group.
 *                                      (When input is NULL, no object is created)
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      FIXME
 *
 *
 *
 */
DYNAMIC_API PEP_STATUS group_create(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager,
        identity_list *memberlist,
        pEp_group **group
    );

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
DYNAMIC_API PEP_STATUS group_join(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *as_member
    );

/**
 *  <!--       group_dissolve()       -->
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
DYNAMIC_API PEP_STATUS group_dissolve(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager
    );

/**
 *  <!--       group_invite_member()       -->
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
DYNAMIC_API PEP_STATUS group_invite_member(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *group_member
    );

/**
 *  <!--       group_remove_member()       -->
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
PEP_STATUS group_remove_member(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *group_member
    );

/**
 *  <!--       group_rating()       -->
 *
 *  @brief      Get the rating for this group - if the caller is the manager, this will return the aggregate rating
 *              of group members. For members, this will return the rating of the group_identity
 *
 *  @param[in]      session             associated session object
 *  @param[in]      group_identity      the pEp_identity object representing the group. Must contain at least
 *                                      a user_id and address
 *  @param[in]      manager             the pEp_identity object representing the member to remove. Must contain
 *                                      a user_id and address
 *  @param[out]     rating              the group rating
 *
 *  @retval         PEP_STATUS_OK       on success
 *                  error               on failure
 *
 *  @ownership      FIXME
 *
 */
DYNAMIC_API PEP_STATUS group_rating(
        PEP_SESSION session,
        pEp_identity *group_identity,
        pEp_identity *manager,
        PEP_rating *rating
    );



/*************************************************************************************************
 * Group query functions
 *************************************************************************************************/

/**
 *  <!--       query_groups()       -->
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

DYNAMIC_API PEP_STATUS query_groups(
        PEP_SESSION session,
        identity_list **groups
    );

/**
 *  <!--       query_group_manager()       -->
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

DYNAMIC_API PEP_STATUS query_group_manager(
        PEP_SESSION session,
        const pEp_identity * const group,
        pEp_identity **manager
    );

/**
 *  <!--       query_group_members()       -->
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

DYNAMIC_API PEP_STATUS query_group_members(
        PEP_SESSION session,
        const pEp_identity * const group,
        identity_list **members
    );


#ifdef __cplusplus
};
#endif

#endif  //LIBPEPADAPTER_GROUP_HH