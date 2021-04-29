// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_STD_UTILS_HXX
#define LIBPEPADAPTER_STD_UTILS_HXX

#include <sstream>

namespace pEp {
    namespace Utils {
        template<typename T>
        std::string to_string(const std::vector<T>& v)
        {
            std::stringstream ss;
            for (const T& elem : v) {
                ss << elem << std::endl;
            }
            return ss.str();
        }
    } // namespace Utils
} // namespace pEp
#endif // LIBPEPADAPTER_STD_UTILS_HXX