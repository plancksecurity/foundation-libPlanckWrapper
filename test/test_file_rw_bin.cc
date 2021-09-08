// This file is under GNU General Public License 3.0
// see LICENSE.txt
#include <iostream>
#include <cassert>

#include "../src/pEpLog.hh"
#include "../src/std_utils.hh"

int main(int argc, char* argv[])
{
    pEp::Adapter::pEpLog::set_enabled(true);
    std::string filename = "test_rw.bin";

    std::vector<char> v_out{};
    for (int i = 0; i < 1000; i++) {
        v_out.push_back(pEp::Utils::random_char(0, 255));
    }

    pEp::Utils::file_write_bin(filename, v_out);

    std::vector<char> v_in = pEp::Utils::file_read_bin(filename);

    assert(v_in == v_out);
}
