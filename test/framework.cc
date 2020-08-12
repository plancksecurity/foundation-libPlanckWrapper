#include "framework.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include <pEp/keymanagement.h>

namespace pEp {
    namespace Test {
        using namespace Adapter;

        void setup(vector<string>& a)
        {
            string dir = "/tmp/test_pEp.XXXXXXXXXXXX";

            if (a.size() > 1) {
                if (a[1] == "--help") {
                    cout << "usage: " << a[0] << " [--dir HOME]" << endl;
                    exit(0);
                }
                else if (a[1] == "--dir" && a.size() == 3) {
                    dir = a[2];
                }
                else {
                    cerr << "illegal parameter" << endl;
                    exit(1);
                }
            }

            char path[MAXPATHLEN+1];
            const char *templ = dir.c_str();
            strcpy(path, templ);
            mkdtemp(path);
            chdir(path);
            setenv("HOME", path, 1);
            cerr << "test directory: " << path << endl;
        }

        void setup(int argc, char **argv)
        {
            vector<string> a{(size_t) argc};
            for (int i=0; i<argc; ++i)
                a[i] = argv[i];

            setup(a);
        }

        void import_key_from_file(string filename)
        {
            ifstream f(filename, ifstream::in);
            string key{istreambuf_iterator<char>(f), istreambuf_iterator<char>()};
            ::identity_list *il = NULL;
            PEP_STATUS status = ::import_key(session(), key.c_str(), key.length(), &il);
            assert(status == PEP_KEY_IMPORTED);
            ::free_identity_list(il);
        }
    };
};
