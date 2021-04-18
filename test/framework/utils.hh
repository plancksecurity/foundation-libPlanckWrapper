// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_UTILS_HH
#define LIBPEPADAPTER_UTILS_HH

#include <string>
#include <pEp/message.h>
#include <pEp/identity_list.h>
#include <pEp/group.h>
#include <exception>

namespace pEp {
    namespace Test {
        namespace Log {
            void logH1(std::string msg);
            void logH2(std::string msg);
        }
        namespace Utils {
            // pEpEngine datatypes to string
            std::string identity_to_string(::pEp_identity *ident, bool full = true, int indent = 0);
            std::string identitylist_to_string(::identity_list *idl, bool full = true, int indent = 0);
            std::string member_to_string(::pEp_member *member, bool full = true, int indent = 0);
            std::string memberlist_to_string(::member_list *mbl, bool full = true, int indent = 0);
            std::string group_to_string(::pEp_group *group, bool full = true, int indent = 0);

            // C++/STL data types to string
            template<typename T>
            std::string vector_to_string(std::vector<T> v);

            // exception utils
            void print_exception(const std::exception& e, int level = 0);

            // file utils
            std::ofstream file_create(const std::string &filename);
            bool file_exists(const std::string &filename);
            void file_delete(const std::string &filename);
            void file_ensure_not_existing(std::string path);
        }
    } // namespace Test
} // namespace pEp

#include "utils.hxx"

#endif // LIBPEPADAPTER_UTILS_HH