#pragma once

#include <pEp/pEpEngine.h>
#include <string>

namespace pEp
{
    // creates a textual string (returned by pep_status_to_string() ) and the numerical status value.
    // in pEpEngine.h positive values are in hex value, negatives in decimal. So we follow this.
    std::string status_to_string(PEP_STATUS status);
    
} // end of namespace pEp
