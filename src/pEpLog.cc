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

using namespace std;

namespace pEp {
    namespace Adapter {
        namespace pEpLog {

            // NON CLASS
            mutex mtx;
            atomic_bool is_enabled{ false };

            void set_enabled(const bool& enabled)
            {
                is_enabled.store(enabled);
            }

            bool get_enabled()
            {
                return is_enabled.load();
            }

            // Common "print" function implementing the actual "backends"
            void _log(const string& msg)
            {
                lock_guard<mutex> l(mtx);
#ifdef ANDROID
                __android_log_print(ANDROID_LOG_DEBUG, "pEpDebugLog", "%s", msg.c_str());
#else
                cerr << msg << endl; //endl also flushes, but cerr is unbuffered anyways
#endif
            }

            void log(const string& msg)
            {
                _log(msg);
            }

            void logH1(const string& msg)
            {
                log(decorateH1(msg));
            }

            void logH2(const string& msg)
            {
                log(decorateH2(msg));
            }

            string decorateH1(const string& msg)
            {
                stringstream tmp;
                char decoration{ '=' };
                tmp << endl
                    << endl
                    << std::string(30, decoration) << ' ' << msg << ' '
                    << std::string(30, decoration) << endl;
                return tmp.str();
            }

            string decorateH2(const string& msg)
            {
                stringstream tmp;
                char decoration{ '-' };
                tmp << endl
                    << std::string(10, decoration) << ' ' << msg << ' '
                    << std::string(10, decoration) << endl;
                return tmp.str();
            }

        } // namespace pEpLog
    }     // namespace Adapter
} // namespace pEp

namespace pEp {
    namespace Adapter {
        namespace pEpLog {
            // Class pEpLogger

            int pEpLogger::auto_instance_nr = 0;
            pEpLogger::pEpLogger(const string& classname, const bool& enabled) :
                classname(classname), is_enabled(enabled)
            {
                auto_instance_nr++;
                this->set_instancename(to_string(auto_instance_nr));
            }

            void pEpLogger::log(const string& msg) const
            {
                std::stringstream msg_;
                msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";
                msg_ << " - ";
                msg_ << this->get_classname() << "[" << this->get_instancename() << "]";
                msg_ << " - " << msg;
                this->logRaw(msg_.str());
            }

            void pEpLogger::logRaw(const string& msg) const
            {
                if (this->is_enabled) {
                    _log(msg);
                }
            }

            void pEpLogger::set_enabled(const bool& enabled)
            {
                this->is_enabled = enabled;
            }

            bool pEpLogger::get_enabled() const
            {
                return this->is_enabled;
            }

            string pEpLogger::get_classname() const
            {
                return this->classname;
            }

            void pEpLogger::set_instancename(const string& name)
            {
                this->instancename = name;
            }

            string pEpLogger::get_instancename() const
            {
                return this->instancename;
            }
        } // namespace pEpLog
    }     // namespace Adapter
} // namespace pEp
