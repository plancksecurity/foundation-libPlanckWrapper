// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_STD_UTILS_HXX
#define LIBPEPADAPTER_STD_UTILS_HXX

#include <sstream>
#include <fstream>
#include <iomanip>
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

        template<class T>
        std::string bin2hex(const T &bin)
        {
            std::string ret{};
            std::stringstream ss{};
            for (const auto &i : bin) {
                ss << std::hex << std::setfill('0') << std::setw(2) << (int)i;
            }
            ret = ss.str();
            return ret;
        }

        template<class T = unsigned char>
        std::vector<T> hex2bin(const std::string &hex_str)
        {
            std::vector<T> ret{};
            if ((hex_str.size() % 2) != 0) {
                throw std::runtime_error("hex2bin: Invalid hex string: must be even length");
            }
            for (size_t i = 0; i < hex_str.size(); i += 2) {
                std::ostringstream val_hex{};
                val_hex << hex_str.at(i);
                val_hex << hex_str.at(i + 1);

                int val_int;
                std::istringstream conv_ss{ val_hex.str() };
                conv_ss >> std::hex >> val_int;
                if (conv_ss.fail()) {
                    throw std::runtime_error("hex2bin: invalid hex string" + hex_str);
                }
                ret.push_back(static_cast<T>(val_int));
            }
            return ret;

            //             alternative way
            //            std::string extract;
            //            for (std::string::const_iterator pos = hex_str.begin(); pos < hex_str.end(); pos += 2) {
            //                extract.assign(pos, pos + 2);
            //                ret.push_back(std::stoi(extract, nullptr, 16));
            //            }
            //            return ret;
        }

    } // namespace Utils
} // namespace pEp
#endif // LIBPEPADAPTER_STD_UTILS_HXX
