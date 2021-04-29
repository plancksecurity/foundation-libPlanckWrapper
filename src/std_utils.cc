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
                runtime_error e{"path_delete_all(\""+path+"\")"};
                throw_with_nested(e);
            }
        }

        ofstream file_create(const string &filename)
        {
            ofstream outfile{ filename };
            return outfile;
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
                remove_if(ret.begin(), ret.end(), [](string elem) { return !path_is_dir(elem); }),
                ret.end());

            return ret;
        }

        vector<string> dir_list_files(const string &dirname)
        {
            vector<string> ret = dir_list_all(dirname);
            ret.erase(
                remove_if(ret.begin(), ret.end(), [](string elem) { return !path_is_dir(elem); }),
                ret.end());
            return ret;
        }

    } // namespace Utils
} // namespace pEp
