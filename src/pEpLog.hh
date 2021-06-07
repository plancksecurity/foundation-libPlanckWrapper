// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PEPLOG_HH
#define LIBPEPADAPTER_PEPLOG_HH

#include <sstream>
#include <thread>
#include <unistd.h>
#include "std_utils.hh"

// pEpLog
// ======
// a "to be kept ultra small and simple" logging unit.
// featuring:
// * Logging macros that completely eliminate any logging calls in release-builds (NDEBUG)
// * thread safe (no interleave when logging from diff threads) TODO: pEpLogger: REALLY?
// * OS dependent backend switches:
//      * android: __android_log_print
//      * all other OS: cerr
// * Logging without any class/object (pEpLog / pEpLogRaw macros)
//      * runtime switchable (on/off) only on a global level
// * Class backed Logging macros (pEpLogClass / pEpLogClassRaw)
// *    * runtime switchable (on/off)  on a class and object level
//
// There are already too many features and you might want even more and more.
// But the feature-policy of this logging unit is very restrictive, and there is a
// primary design goal to keep it very simple, maintainable and portable.
//

// pEpLog - logformat "thread - __FILE__::__FUNTION__ - <message>"
// To be used in a non-class/object context
#ifdef NDEBUG
    #define pEpLog(msg)                                                                            \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLog(msg)                                                                            \
        do {                                                                                       \
            if (pEp::Adapter::pEpLog::get_enabled()) {                                             \
                std::stringstream msg_;                                                            \
                msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";               \
                msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                 \
                msg_ << " - " << msg;                                                              \
                pEp::Adapter::pEpLog::log(msg_.str());                                             \
            }                                                                                      \
        } while (0)
#endif // NDEBUG

// pEpLogH1 - logformat "Thread - __FILE__::__FUNTION__ - <=============== message ==============>"
#ifdef NDEBUG
    #define pEpLogH1(msg)                                                                          \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogH1(msg)                                                                          \
        do {                                                                                       \
            if (pEp::Adapter::pEpLog::get_enabled()) {                                             \
                std::stringstream msg_;                                                            \
                msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";               \
                msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                 \
                msg_ << " - " << pEp::Adapter::pEpLog::decorateH1(msg);                            \
                pEp::Adapter::pEpLog::log(msg_.str());                                             \
            }                                                                                      \
        } while (0)
#endif // NDEBUG

// pEpLogH1 - logformat "Thread - __FILE__::__FUNTION__ - <--------------- message -------------->"
#ifdef NDEBUG
    #define pEpLogH2(msg)                                                                          \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogH2(msg)                                                                          \
        do {                                                                                       \
            if (pEp::Adapter::pEpLog::get_enabled()) {                                             \
                std::stringstream msg_;                                                            \
                msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";               \
                msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                 \
                msg_ << " - " << pEp::Adapter::pEpLog::decorateH2(msg);                            \
                pEp::Adapter::pEpLog::log(msg_.str());                                             \
            }                                                                                      \
        } while (0)
#endif // NDEBUG


// RAW == without prefix of thread, file, function
// pEpLogRaw - logformat "<message>"
#ifdef NDEBUG
    #define pEpLogRaw(msg)                                                                         \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogRaw(msg)                                                                         \
        do {                                                                                       \
            if (pEp::Adapter::pEpLog::get_enabled()) {                                             \
                pEp::Adapter::pEpLog::log(msg_.str());                                             \
            }                                                                                      \
        } while (0)
#endif // NDEBUG

// pEpLogRawH1 - logformat "<--------------- message -------------->"
#ifdef NDEBUG
    #define pEpLogRawH1(msg)                                                                       \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogRawH1(msg)                                                                       \
        do {                                                                                       \
            if (pEp::Adapter::pEpLog::get_enabled()) {                                             \
                pEp::Adapter::pEpLog::logH1(msg_.str());                                           \
            }                                                                                      \
        } while (0)
#endif // NDEBUG

// pEpLogRawH2 - logformat <=============== message ==============>"
#ifdef NDEBUG
    #define pEpLogRawH2(msg)                                                                       \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogRawH2(msg)                                                                       \
        do {                                                                                       \
            if (pEp::Adapter::pEpLog::get_enabled()) {                                             \
                pEp::Adapter::pEpLog::logH2(msg_.str());                                           \
            }                                                                                      \
        } while (0)
#endif // NDEBUG


namespace pEp {
    namespace Adapter {
        namespace pEpLog {
            // Logging functions to control pEpLog() macro
            void set_enabled(const bool& is_enabled);
            bool get_enabled();
            void log(const std::string& msg, Utils::Color col = Utils::Color::WHITE);
            void logH1(const std::string& msg, Utils::Color col = Utils::Color::WHITE);
            void logH2(const std::string& msg, Utils::Color col = Utils::Color::WHITE);
            void logH3(const std::string&msg, Utils::Color col = Utils::Color::WHITE);
            std::string decorate_three_lines(const std::string& msg, char decoration = '-');
            std::string decorate_centered(const std::string& msg, char decoration = '-');
        } // namespace pEpLog
    }     // namespace Adapter
} // namespace pEp

// --------------------------------------------------------------------------------------------------


// pEpLogClass is to be used in a class
// pEpLogger can only print the "thread - file::class::function - <message>" format using this macro
// WARNING: Some magic is needed
// Usage:
// create your logger obj in your class as a public member (usually)
// Adapter::pEpLog::pEpLogger logger{"<CLASSNAME>", enabled: true|false};
// then, create an alias for your logger called "m4gic_logger_n4me" as a private member
// Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger;
// Thats all.
// Now in your implementation, to log a message you just write:
// pEpLogClass("my great logging message");
#ifdef NDEBUG
    #define pEpLogClass(msg)                                                                       \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogClass(msg)                                                                       \
        do {                                                                                       \
            std::stringstream msg_;                                                                \
            msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";                   \
            msg_ << " - " << this->m4gic_logger_n4me.get_classname();                              \
            msg_ << "[" << this->m4gic_logger_n4me.get_instancename() << "]";                      \
            msg_ << "::" << __FUNCTION__;                                                          \
            msg_ << " - " << (msg);                                                                \
            this->m4gic_logger_n4me.logRaw(msg_.str());                                            \
        } while (0)
#endif // NDEBUG

// pEpLogClassRaw is the same as pEpLogClass, but does not print anything except the supplied msg
// This can also be achieved without this macro, just use the log method of pEpLogger
// You also need to set up the logger in your class as for pEpLogClass
// The only advantage of this macro is that is compiled away to nothing with NDEBUG
#ifdef NDEBUG
    #define pEpLogClassRaw(msg)                                                                    \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogClassRaw(msg)                                                                    \
        do {                                                                                       \
            this->m4gic_logger_n4me.logRaw(msg);                                                   \
        } while (0)
#endif // NDEBUG

namespace pEp {
    namespace Adapter {
        namespace pEpLog {
            class pEpLogger {
            public:
                pEpLogger() = delete;
                pEpLogger(const std::string& classname, const bool& enabled);
                // Print a logging message in the format "thread - classname[instancename] - <msg>"
                void log(const std::string& msg, Utils::Color col = Utils::Color::WHITE) const;
                // Prints just "<msg>"
                void logRaw(const std::string& msg,  Utils::Color col = Utils::Color::WHITE) const;
                void set_enabled(const bool& enabled);
                bool get_enabled() const;
                std::string get_classname() const;
                // If never set, the default instancename is a unique number
                void set_instancename(const std::string& name);
                std::string get_instancename() const;

            private:
                static int auto_instance_nr;
                bool is_enabled;
                std::string classname;
                std::string instancename;
            };
        } // namespace pEpLog
    }     // namespace Adapter
} // namespace pEp


#endif // LIBPEPADAPTER_PEPLOG_HH
