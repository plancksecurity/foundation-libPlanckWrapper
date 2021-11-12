#include "../src/std_utils.hh"
#include "../src/pEpLog.hh"
#include <iostream>
#include <cassert>
#include <pEp/pitytest11/PityTest.hh>

using namespace pEp;

int main()
{
    pEp::Adapter::pEpLog::set_enabled(true);

    {
        // Valid hex string
        std::string str_in{ "FFABCD00EF123200" };
        std::vector<unsigned char> bin = Utils::hex2bin<unsigned char>(str_in);
        PITYASSERT(str_in.length() == bin.size() * 2, "Size error");

        std::string str_out = pEp::Utils::bin2hex(bin);
        pEpLog("Hex_IN  : '" + Utils::to_lower(str_in) + "'");
        pEpLog("Hex_OUT : '" + Utils::to_lower(str_out) + "'");

        PITYASSERT(Utils::to_lower(str_in) == Utils::to_lower(str_out), "roundtrip failed");
    }

    {
        // Uneven string throws
        std::string str_in{ "FFA" };
        PITYASSERT_THROWS(Utils::hex2bin<char>(str_in), "Uneven string should throw");
    }

    {
        // Non-hex chars
        std::string str_in{ "pEp!" };
        PITYASSERT_THROWS(Utils::hex2bin<char>(str_in), "Invalid hex chars should throw");
    }

    pEpLog("All tests passsed");
}