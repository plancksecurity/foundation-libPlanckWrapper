// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_STD_UTILS_HH
#define LIBPEPADAPTER_STD_UTILS_HH

#include <string>
#include <exception>
#include <vector>

namespace pEp {
    namespace Utils {
        // C-types helpers
        bool is_c_str_empty(const char *str);

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

        bool path_exists(const std::string &filename);
        bool path_is_dir(const std::string &path);
        void path_delete(const std::string &filename);
        void path_delete_all(const std::string &path);

        std::ofstream file_create(const std::string &filename);
        void path_ensure_not_existing(const std::string &path);

        void dir_create(const std::string &dirname, const mode_t mode = 0775);
        std::vector<std::string> dir_list_all(
            const std::string &path,
            const bool incl_dot_and_dotdot = false);
        std::vector<std::string> dir_list_dirs(
            const std::string &dirname,
            const bool incl_dot_and_dotdot = false);
        std::vector<std::string> dir_list_files(const std::string &dirname);
    } // namespace Utils
} // namespace pEp

#include "std_utils.hxx"

#endif // LIBPEPADAPTER_STD_UTILS_HH