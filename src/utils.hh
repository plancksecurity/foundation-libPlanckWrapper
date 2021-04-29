// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_UTILS_HH
#define LIBPEPADAPTER_UTILS_HH

#include "pEpLog.hh"
#include <pEp/message.h>
#include <pEp/identity_list.h>
#include <pEp/group.h>
#include <exception>
#include <vector>
#include <string>

namespace pEp {
    namespace Utils {
        // C-types to C++ types
        std::vector<::pEp_identity *> to_cxx(const ::identity_list &idl);

        // pEpEngine datatypes to string
        std::string to_string(const ::pEp_identity *const ident, bool full = true, int indent = 0);
        std::string to_string(const ::identity_list *const idl, bool full = true, int indent = 0);
        std::string to_string(const ::pEp_member *const member, bool full = true, int indent = 0);
        std::string to_string(const ::member_list *const mbl, bool full = true, int indent = 0);
        std::string to_string(const ::pEp_group *const group, bool full = true, int indent = 0);

    } // namespace Utils
} // namespace pEp
#endif // LIBPEPADAPTER_UTILS_HH