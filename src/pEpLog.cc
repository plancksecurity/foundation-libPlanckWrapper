// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "pEpLog.hh"
#include <iostream>
#include <sstream>
#include <mutex>
#include <atomic>

#ifdef ANDROID
    #include <android/log.h>
#endif


namespace pEp {
    namespace Adapter {
        namespace pEpLog {

            std::mutex mtx;

            std::atomic_bool is_enabled{ false };

            void set_enabled(bool enabled)
            {
                is_enabled.store(enabled);
            }

            bool get_enabled()
            {
                return is_enabled.load();
            }

            void log(std::string msg)
            {
                if (is_enabled.load()) {
                    std::lock_guard<std::mutex> l(mtx);
#ifdef ANDROID
                    __android_log_print(ANDROID_LOG_DEBUG, "pEpDebugLog", "%s", msg.c_str());
#else
                    std::cout << msg << std::endl; //std::endl also flushes
#endif
                }
            }

        } // namespace pEpLog
    }     // namespace Adapter
} // namespace pEp
