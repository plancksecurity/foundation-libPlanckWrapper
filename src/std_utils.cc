// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "std_utils.hh"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cerrno>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <thread>
#include <random>

using namespace std;
using namespace pEp;

namespace pEp {
    namespace Utils {
        bool is_c_str_empty(const char *str)
        {
            if (str == nullptr) {
                return true;
            }
            string tmp{ str };
            if (tmp.empty()) {
                return true;
            }
            return false;
        }

        string nested_exception_to_string(const exception &e, int level, string src)
        {
            src += string(level, ' ') + "exception: " + e.what() + "\n";
            try {
                rethrow_if_nested(e);
            } catch (const exception &e) {
                src = nested_exception_to_string(e, level + 1, src);
            } catch (...) {
            }
            return src;
        }

        //            void print_exception(const exception &e, int level)
        //            {
        //                cerr << string(level, ' ') << "exception: " << e.what() << endl;
        //                try {
        //                    rethrow_if_nested(e);
        //                } catch (const exception &e) {
        //                    print_exception(e, level + 1);
        //                } catch (...) {
        //                }
        //            }


        // File utils
        bool path_exists(const string &filename)
        {
            ifstream ifile(filename.c_str());
            return (bool)ifile;
        }

        bool path_is_dir(const string &path)
        {
            bool ret = false;
            struct stat statbuf;
            if (stat(path.c_str(), &statbuf) != 0) {
                runtime_error e{ "path_is_dir(\"" + path + "\") -  " + strerror(errno) };
                throw(e);
            }
            if (S_ISDIR(statbuf.st_mode)) {
                ret = true;
            }
            return ret;
        }

        void path_delete(const string &filename)
        {
            int status = remove(filename.c_str());
            if (status) {
                runtime_error e{ string("path_delete(\"" + filename + "\") -  " + strerror(errno)) };
                throw(e);
            }
        }

        void path_delete_all(const string &path)
        {
            try {
                if (!path_is_dir(path)) {
                    path_delete(path);
                } else {
                    vector<string> dirlist = dir_list_all(path);
                    if (dirlist.empty()) {
                        path_delete(path);
                    } else {
                        for (const string &filename : dirlist) {
                            string newpath = path + "/" + filename;
                            path_delete_all(newpath);
                        }
                        path_delete(path);
                    }
                }
            } catch (...) {
                runtime_error e{ "path_delete_all(\"" + path + "\")" };
                throw_with_nested(e);
            }
        }

        ofstream file_create(const string &filename)
        {
            ofstream outfile{ filename };
            return outfile;
        }

        std::string file_read(const std::string &filename)
        {
            auto ss = ostringstream{};
            ifstream input_file(filename);
            if (!input_file.is_open()) {
                runtime_error e{ "Could not open the file: " + filename };
                exit(EXIT_FAILURE);
            }
            ss << input_file.rdbuf();
            return ss.str();
        }

        void path_ensure_not_existing(const string &path)
        {
            while (path_exists(path)) {
                path_delete(path);
            }
        }

        void dir_create(const string &dirname, const mode_t mode)
        {
            if (mkdir(dirname.c_str(), mode) != 0) {
                runtime_error e{ string("dir_create(\"" + dirname + "\") -  " + strerror(errno)) };
                throw(e);
            }
        }


        void dir_ensure(const std::string &path)
        {
            if (!Utils::path_exists(path)) {
                Utils::dir_create(path);
            }
        }

        void dir_recreate(const std::string &path)
        {
            if (Utils::path_exists(path)) {
                Utils::path_delete_all(path);
            }
            Utils::dir_create(path);
        }

        vector<string> dir_list_all(const std::string &path, const bool incl_dot_and_dotdot)
        {
            vector<string> ret;
            if (!path_exists(path)) {
                runtime_error e{ "dir_list_all(\"" + path + "\") -  Error: does not exist" };
                throw(e);
            }

            if (!path_is_dir(path)) {
                runtime_error e{ "dir_list_all(\"" + path + "\") -  Error: is not a directory" };
                throw(e);
            }

            DIR *dirp = opendir(path.c_str());
            if (dirp == nullptr) {
                runtime_error e{ "dir_list_all(\"" + path + "\") -  Error opening dir" };
                throw e;
            }

            struct dirent *dp;
            while ((dp = readdir(dirp)) != NULL) {
                ret.push_back(string(dp->d_name));
            }

            if (!incl_dot_and_dotdot) {
                ret.erase(
                    remove_if(
                        ret.begin(),
                        ret.end(),
                        [](string elem) { return (elem == "." || elem == ".."); }),
                    ret.end());
            }

            closedir(dirp);
            return ret;
        }

        vector<string> dir_list_dirs(const string &dirname, const bool incl_dot_and_dotdot)
        {
            vector<string> ret = dir_list_all(dirname, incl_dot_and_dotdot);
            ret.erase(
                remove_if(
                    ret.begin(),
                    ret.end(),
                    [dirname](string elem) { return !path_is_dir(dirname + "/" + elem); }),
                ret.end());

            return ret;
        }

        vector<string> dir_list_files(const string &dirname)
        {
            vector<string> ret = dir_list_all(dirname);
            ret.erase(
                remove_if(
                    ret.begin(),
                    ret.end(),
                    [dirname](string elem) { return path_is_dir(dirname + "/" + elem); }),
                ret.end());
            return ret;
        }

        // Attention, it pads left...
        string padTo(const string &str, const size_t num, const char paddingChar)
        {
            string ret{ str };
            if (num > ret.size()) {
                ret.insert(0, num - ret.size(), paddingChar);
            }
            return ret;
        }

        string to_termcol(const Color &col)
        {
            switch (col) {
                case Color::RESET:
                    return "\033[0m";
                case Color::BLACK:
                    return "\033[30m";
                case Color::RED:
                    return "\033[31m";
                case Color::GREEN:
                    return "\033[32m";
                case Color::YELLOW:
                    return "\033[33m";
                case Color::BLUE:
                    return "\033[34m";
                case Color::MAGENTA:
                    return "\033[35m";
                case Color::CYAN:
                    return "\033[36m";
                case Color::WHITE:
                    return "\033[37m";
                default:
                    return "\033[0m";
            }
        }

        void sleep_millis(int milis) {
            std::chrono::milliseconds timespan(milis);
            std::this_thread::sleep_for(timespan);
        }

        unsigned char random_char(unsigned char min, unsigned char max)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<short> dis(static_cast<short>(min), static_cast<short>(max));
            return static_cast<unsigned char>(dis(gen));
        }

        std::string random_string(unsigned char min, unsigned char max, int len)
        {
            std::stringstream ret;
            for (int i = 0; i < len; i++) {
                ret << random_char(97, 122);
            }
            return ret.str();
        }

    } // namespace Utils
} // namespace pEp
