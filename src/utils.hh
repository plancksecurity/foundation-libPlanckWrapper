// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_UTILS_HH
#define LIBPEPADAPTER_UTILS_HH

#include <pEp/pEpLog.hh>
#include <pEp/message.h>
#include <pEp/identity_list.h>
#include <pEp/group.h>
#include <exception>
#include <vector>
#include <string>

namespace pEp {
    namespace Utils {
        // C-types to C++ types
        std::vector<::pEp_identity*> to_cxx(const ::identity_list& idl);

        // pEpEngine datatypes to string
        std::string to_string(const ::pEp_identity* ident, bool full = true, int indent = 0);
        std::string to_string(const ::bloblist_t* ident, bool full = true, int indent = 0);
        std::string to_string(const ::stringpair_list_t* spl, bool full = true, int indent = 0);
        std::string to_string(const ::message* msg, bool full = true, int indent = 0);
        std::string to_string(const ::identity_list* idl, bool full = true, int indent = 0);
        std::string to_string(const ::pEp_member* member, bool full = true, int indent = 0);
        std::string to_string(const ::member_list* mbl, bool full = true, int indent = 0);
        std::string to_string(const ::pEp_group* group, bool full = true, int indent = 0);

        // Misc
        std::string readKey(); // TODO: Move to std_utils

    } // namespace Utils
} // namespace pEp

// ostream inserters
std::ostream& operator<<(std::ostream& o, const ::pEp_identity* pEp_c_dt);
std::ostream& operator<<(std::ostream& o, const ::bloblist_t* pEp_c_dt);
std::ostream& operator<<(std::ostream& o, const ::stringpair_list_t* pEp_c_dt);
std::ostream& operator<<(std::ostream& o, const ::message* pEp_c_dt);
std::ostream& operator<<(std::ostream& o, const ::identity_list* pEp_c_dt);
std::ostream& operator<<(std::ostream& o, const ::pEp_member* pEp_c_dt);
std::ostream& operator<<(std::ostream& o, const ::member_list* pEp_c_dt);
std::ostream& operator<<(std::ostream& o, const ::pEp_group* pEp_c_dt);

#endif // LIBPEPADAPTER_UTILS_HH