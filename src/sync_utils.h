// This file is under GNU General Public License 3.0
// see LICENSE.txt
// clang-format off
#ifndef LIBPEPADAPTER_SYNC_UTILS_H
#define LIBPEPADAPTER_SYNC_UTILS_H

#include <pEp/message_api.h>
#include <pEp/Sync_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief         Evaluate condition: Is deviceGrouped true?
* @param[in]     session         the session
* @param[out]    result          true if deviceGrouped, else false
* @retval        status
*/
PEP_STATUS deviceGrouped(PEP_SESSION session, bool * result);


#ifdef __cplusplus
}
#endif

#endif //LIBPEPADAPTER_SYNC_UTILS_H