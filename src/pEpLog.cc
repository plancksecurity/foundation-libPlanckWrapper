// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "pEpLog.hh"
#include <iostream>
#include <sstream>
#include <mutex>
#include <atomic>
#include <cmath>
#include "std_utils.hh"

#ifdef ANDROID
    #include <android/log.h>
#endif

using namespace std;

namespace pEp {
    namespace Adapter {
        namespace pEpLog {
            int line_width = 120;

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
            void _log(const string& msg, Utils::Color col = Utils::Color::WHITE)
            {
                lock_guard<mutex> l(mtx);
#ifdef ANDROID
                __android_log_print(ANDROID_LOG_DEBUG, "pEpDebugLog", "%s", msg.c_str());
#else
                cerr << Utils::to_termcol(col) << msg << Utils::to_termcol(Utils::Color::RESET)
                     << endl; //endl also flushes, but cerr is unbuffered anyways
#endif
            }

            void log(const string& msg, Utils::Color col)
            {
                _log(msg, col);
            }

            void logH1(const string& msg, Utils::Color col)
            {
                log(decorate_three_lines(msg, '='), col);
            }

            void logH2(const string& msg, Utils::Color col)
            {
                log(decorate_centered(msg, '='), col);
            }

            void logH3(const string& msg, Utils::Color col)
            {
                log(decorate_centered(msg, '-'), col);
            }

            string decorate_three_lines(const string& msg, char decoration)
            {
                stringstream tmp;
                tmp << std::string(line_width, decoration) << endl
                    << msg << endl
                    << std::string(line_width, decoration);
                return tmp.str();
            }

            string decorate_centered(const string& msg, char decoration)
            {
                stringstream tmp;
                int max_len = line_width - 10;
                // truncate msg
                string msg_truncated = msg;
                if (msg.length() >= max_len) {
                    msg_truncated = msg.substr(0, max_len);
                    msg_truncated += "...";
                }

                // define decolen
                int decolen = static_cast<int>(
                    floor((double(line_width - msg_truncated.length()))) / 2.0);

                tmp << endl
                    << std::string(decolen, decoration) << ' ' << msg_truncated << ' '
                    << std::string(decolen, decoration);
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

            void pEpLogger::log(const string& msg, Utils::Color col) const
            {
                std::stringstream msg_;
                msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";
                msg_ << " - ";
                msg_ << this->get_classname() << "[" << this->get_instancename() << "]";
                msg_ << " - " << msg;
                this->logRaw(msg_.str(), col);
            }

            void pEpLogger::logRaw(const string& msg, Utils::Color col) const
            {
                if (this->is_enabled) {
                    _log(msg, col);
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
