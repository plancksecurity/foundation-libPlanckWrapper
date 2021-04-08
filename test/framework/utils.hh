// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_UTILS_HH
#define LIBPEPADAPTER_UTILS_HH

#include <string>
#include <pEp/message.h>
#include <pEp/group.h>

namespace pEp {
    namespace Test {
        namespace Utils {
            std::string identity_to_string(::pEp_identity* ident, bool full = true, int indent = 0);
            std::string member_to_string(::pEp_member* member, bool full = true, int indent = 0);
            std::string memberlist_to_string(::member_list* mbl, bool full = true, int indent = 0);
            std::string group_to_string(::pEp_group* group, bool full = true, int indent = 0);
        }
    } // namespace Test
} // namespace pEp
#endif