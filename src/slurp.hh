// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_LIB_SLURP_HH
#define LIBPEPADAPTER_LIB_SLURP_HH

#include <string>

namespace pEp {
    // reads a whole file and returns it as std::string
    // throws std::runtime_error() if the file cannot be read. Empty file is not an error.
    std::string slurp(const std::string& filename);

} // end of namespace pEp

#endif // LIBPEPADAPTER_LIB_SLURP_HH