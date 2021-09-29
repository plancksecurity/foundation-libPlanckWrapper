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

        // File utils
        // ----------
        // path (file & dir)
        bool path_exists(const std::string &filename);
        void path_delete(const std::string &filename);

#ifndef WIN32
        bool path_is_dir(const std::string &path);
        void path_delete_all(const std::string &path);
        std::string path_dirname(std::string const& f);
        std::string path_get_abs(const std::string& path);
#endif
        void path_ensure_not_existing(const std::string &path);

        // file
        std::ofstream file_create(const std::string &filename);
        std::string file_read(const std::string &filename);

        template<typename T = char>
        std::vector<T> file_read_bin(const std::string& filename);

        template<typename T = char>
        void file_write_bin(const std::string& filename, std::vector<T>& data);

        // dir
#ifndef WIN32
        void dir_create(const std::string &dirname, const mode_t mode = 0775);
        void dir_ensure(const std::string &path);
        void dir_recreate(const std::string &path);

        std::vector<std::string> dir_list_all(
            const std::string &path,
            const bool incl_dot_and_dotdot = false);

        std::vector<std::string> dir_list_dirs(
            const std::string &dirname,
            const bool incl_dot_and_dotdot = false);

        std::vector<std::string> dir_list_files(const std::string &dirname);
#endif

        //String tools
        std::string padTo(const std::string &str, const size_t num, const char paddingChar);
        std::string clip(const std::string &str, const size_t len);
        std::string tldr(const std::string &str, const size_t len);
        std::string to_lower(const std::string& data);

        enum class Color
        {
            RESET,
            BLACK,
            RED,
            GREEN,
            YELLOW,
            BLUE,
            MAGENTA,
            CYAN,
            WHITE,
        };

        std::string to_termcol(const Color &col);


        // Time
        void sleep_millis(int milis);

        // Random
        unsigned char random_char(unsigned char min, unsigned char max);
        std::string random_string(unsigned char min, unsigned char max, int len);

    } // namespace Utils
} // namespace pEp

#include "std_utils.hxx"

#endif // LIBPEPADAPTER_STD_UTILS_HH