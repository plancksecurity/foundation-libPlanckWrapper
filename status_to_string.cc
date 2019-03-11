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
    return ss.str() + " \"" + pep_status_to_string(status) + '"';
}
    
} // end of namespace pEp
