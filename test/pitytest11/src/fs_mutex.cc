#include "fs_mutex.hh"
#include "../../../src/std_utils.hh"
#include<fstream>


namespace pEp {
    namespace PityTest11 {
        fs_mutex::fs_mutex(std::string mutexpath) : mutexpath{ mutexpath } {}

        void fs_mutex::aquire() const
        {
            if (mutexpath.empty()) {
                throw std::runtime_error("no mutexpath set");
            } else {
                std::string mutex_file = mutexpath;
                while (Utils::path_exists(mutex_file)) {
                    Utils::sleep_millis(2);
                }
                std::ofstream msgfile = Utils::file_create(mutexpath);
            }
        }

        void fs_mutex::release() const
        {
            if (mutexpath.empty()) {
                throw std::runtime_error("no mutexpath set");
            } else {

                try {
                    Utils::path_delete(mutexpath);
                    // Give others a chance to pickup
                    Utils::sleep_millis(4);
                } catch (...) {
                    //                    pEpLogClass("Error releasing fsmutex");
                }
            }
        }

    } // namespace PityTest
} // namespace pEp

