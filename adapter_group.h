// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_GROUP_HH
#define LIBPEPADAPTER_GROUP_HH


#ifdef __cplusplus
extern "C" {
#endif


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
        pEp_identity *group,
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
        pEp_identity *group,
        identity_list **members
    );


#ifdef __cplusplus
};
#endif

#endif  //LIBPEPADAPTER_GROUP_HH