// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PEPLOG_HH
#define LIBPEPADAPTER_PEPLOG_HH

#include <sstream>
#include <thread>
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
//      * runtime switchable only on a global level
// * Class backed Logging macros (pEpLogClass / pEpLogRawClass)
// *    * runtime switchable logging on a class and object level
//
// There are already too mnay features and you might want even more and more,
// but the feature-policy of this logging unit is very restrictive, and there is a
// primary design goal to keep it very simple, maintainable and portable.
//

// pEpLog is to be used in a non-class/object context
#ifdef NDEBUG
    #define pEpLog(msg)                                                                            \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLog(msg)                                                                            \
        do {                                                                                       \
            std::stringstream msg_;                                                                \
            msg_ << std::this_thread::get_id();                                                    \
            msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                     \
            msg_ << " - " << msg;                                                                  \
            pEp::Adapter::pEpLog::log(msg_.str());                                                 \
        } while (0)
#endif // NDEBUG

// pEpLogRaw the same as pEpLog, but does not print anything except the supplied msg
#ifdef NDEBUG
    #define pEpLogRaw(msg)                                                                         \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogRaw(msg)                                                                         \
        do {                                                                                       \
            pEp::Adapter::pEpLog::log(msg_.str());                                                 \
        } while (0)
#endif // NDEBUG

namespace pEp {
    namespace Adapter {
        namespace pEpLog {
            // Logging functions to control pEpLog() macro
            void log(const std::string& msg);
            void set_enabled(const bool& is_enabled);
            bool get_enabled();
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
// then, create an alias for your logger called "m4gic_logger_n4ame" as a private member
// Adapter::pEpLog::pEpLogger& m4gic_logger_n4ame = logger;
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
            msg_ << std::this_thread::get_id();                                                    \
            msg_ << " - " << this->m4gic_logger_n4ame.get_classname();                             \
            msg_ << "[" << this->m4gic_logger_n4ame.get_instancename() << "]";                     \
            msg_ << "::" << __FUNCTION__;                                                          \
            msg_ << " - " << (msg);                                                                \
            this->m4gic_logger_n4ame.logRaw(msg_.str());                                           \
        } while (0)
#endif // NDEBUG

// pEpLogRawClass is the same as pEpLogClass, but does not print anything except the supplied msg
// This can also be achieved without this macro, just use the log method of pEpLogger
// You also need to set up the logger in your class as for pEpLogClass
// The only advantage of this macro is that is compiled away to nothing with NDEBUG
#ifdef NDEBUG
    #define pEpLogRawClass(msg)                                                                    \
        do {                                                                                       \
        } while (0)
#else
    #define pEpLogRawClass(msg)                                                                    \
        do {                                                                                       \
            this->m4gic_logger_n4ame.logRaw(msg_.str());                                           \
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
                void log(const std::string& msg) const;
                // Prints just "<msg>"
                void logRaw(const std::string& msg) const;
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
