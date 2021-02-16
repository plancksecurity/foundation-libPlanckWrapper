// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "slurp.hh"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace pEp
{

std::string slurp(const std::string& filename)
{
    std::ifstream input(filename.c_str(), std::ios_base::binary);
    if(!input)
    {
        throw std::runtime_error("Cannot read file \"" + filename + "\"! ");
    }
    
    std::stringstream sstr;
    sstr << input.rdbuf();
    return sstr.str();
}

} // end of namespace pEp
