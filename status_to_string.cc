// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "status_to_string.hh"
#include <sstream>


namespace pEp
{

// in pEpEngine.h positive values are hex, negative are decimal. :-o
// TODO: the code should be generated!
std::string status_to_string(PEP_STATUS status)
{
    std::stringstream ss;
    if(status>0)
    {
        ss << "0x" << std::hex << status;
    }else{
        ss << status;
    }
    return ss.str() + " \"" + pEp_status_to_string(status) + '"';
}
    
} // end of namespace pEp
