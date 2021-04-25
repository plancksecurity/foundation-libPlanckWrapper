// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_UTILS_HH
#define LIBPEPADAPTER_UTILS_HH

#include "pEpLog.hh"
#include <string>
#include <pEp/message.h>
#include <pEp/identity_list.h>
#include <pEp/group.h>
#include <exception>
#include <vector>

namespace pEp {
    namespace Utils {
        // C-types to C++ types
        std::vector<::pEp_identity *> to_cxx(const ::identity_list &idl);

        // C-types helpers
        bool is_c_str_empty(const char *str);

        // pEpEngine datatypes to string
        std::string to_string(const ::pEp_identity *const ident, bool full = true, int indent = 0);
        std::string to_string(const ::identity_list *const idl, bool full = true, int indent = 0);
        std::string to_string(const ::pEp_member *const member, bool full = true, int indent = 0);
        std::string to_string(const ::member_list *const mbl, bool full = true, int indent = 0);
        std::string to_string(const ::pEp_group *const group, bool full = true, int indent = 0);

        // C++/STL data types to string
        template<typename T>
        std::string to_string(const std::vector<T> &v);

        // exception utils
        std::string nested_exception_to_string(
            const std::exception &e,
            int level = 0,
            std::string src = "");
        void print_exception(const std::exception &e, int level = 0);

        // file utils
        std::ofstream file_create(const std::string &filename);
        bool file_exists(const std::string &filename);
        void file_delete(const std::string &filename);
        void file_ensure_not_existing(const std::string &path);
    } // namespace Utils
} // namespace pEp

#include "utils.hxx"

#endif // LIBPEPADAPTER_UTILS_HH