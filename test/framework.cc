#include "framework.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include <pEp/keymanagement.h>
#include "Adapter.hh"

namespace pEp {
    namespace Test {
        using namespace Adapter;

        void setup()
        {
            char path[MAXPATHLEN+1];
            const char *templ = "/tmp/test_pEp.XXXXXXXXXXXX";
            strcpy(path, templ);
            char *tmpdir = mkdtemp(path);
            assert(tmpdir);
            chdir(tmpdir);
            setenv("HOME", path, 1);
            cerr << "test directory: " << path << endl;
 
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
