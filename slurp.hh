#ifndef PEP_LIB_SLURP_HH
#define PEP_LIB_SLURP_HH

#include <string>

namespace pEp
{
    // reads a whole file and returns it as std::string
    // throws std::runtime_error() if the file cannot be read. Empty file is not an error.
    std::string slurp(const std::string& filename);

} // end of namespace pEp

#endif // PEP_LIB_SLURP_HH