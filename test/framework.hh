#pragma once

#include <string>
#include <vector>

#include "Adapter.hh"

namespace pEp {
    namespace Test {
        using namespace std;

        void setup(vector<string>& a);
        void setup(int argc=1, char **argv=nullptr);
        void import_key_from_file(string filename);
    };
};

