// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_UTILS_HXX
#define LIBPEPADAPTER_UTILS_HXX

#include <sstream>

namespace pEp {
    namespace Test {
        namespace Utils {
            template<typename T>
            std::string vector_to_string(std::vector<T> v)
            {
                std::stringstream ss;
                for (const T& elem : v) {
                    ss << elem << std::endl;
                }
                return ss.str();
            }
        }
    } // namespace Test
} // namespace pEp
#endif // LIBPEPADAPTER_UTILS_HXX