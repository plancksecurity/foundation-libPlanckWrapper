// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_STD_UTILS_HXX
#define LIBPEPADAPTER_STD_UTILS_HXX

#include <sstream>
#include <fstream>
#include <iterator>

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

        template<typename T>
        std::vector<T> file_read_bin(const std::string &filename)
        {
            std::vector<T> ret{};
            if (pEp::Utils::path_exists(filename)) {
                std::ifstream ifs(filename, std::ios_base::binary);
                ifs.unsetf(std::ios_base::skipws);

                if (ifs.bad()) {
                    throw std::runtime_error("failed to read file: '" + filename + "'");
                }
                ret = { std::istream_iterator<T>(ifs), std::istream_iterator<T>() };
            } else {
                throw std::runtime_error("File does not exist: '" + filename + "'");
            }
            return ret;
        }

        template<typename T>
        void file_write_bin(const std::string &filename, std::vector<T> &data)
        {
            std::fstream f(filename, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
            f.write(data.data(), static_cast<std::streamsize>(data.size()));
            if (f.bad()) {
                throw std::runtime_error("failed to write file: '" + filename + "'");
            }
        }

    } // namespace Utils
} // namespace pEp
#endif // LIBPEPADAPTER_STD_UTILS_HXX
