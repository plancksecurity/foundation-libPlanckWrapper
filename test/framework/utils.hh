// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_TEST_UTILS_HH
#define LIBPEPADAPTER_TEST_UTILS_HH

#include "../../src/pEpLog.hh"
#include <string>
#include <exception>
#include <chrono>
#include <thread>
#include <unistd.h>

// ------------------------------------------------------------------------------------------------

#ifndef ASSERT_EXCEPT
    #define ASSERT_EXCEPT(func)                                                                    \
        do {                                                                                       \
            try {                                                                                  \
                (func);                                                                            \
                assert(false);                                                                     \
            } catch (const exception &e) {                                                         \
                pEp::Adapter::pEpLog::log(nested_exception_to_string(e));                          \
            }                                                                                      \
        } while (0)
#endif

// ------------------------------------------------------------------------------------------------
// Logging macros for testing
// ------------------------------------------------------------------------------------------------
// Use the macros if you need the message to be prefixed with "thread - __FILE__::__FUNTION__"
// OTHERWISE, just use the logging functions from pEp::Adapter::pEpLog

// TESTLOG - logformat "thread - __FILE__::__FUNTION__ - <message>"
// To be used in a non-class/object context
#ifndef TESTLOG
    #define TESTLOG(msg)                                                                           \
        do {                                                                                       \
            std::stringstream msg_;                                                                \
            msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";                 \
            msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                     \
            msg_ << " - " << msg;                                                                  \
            pEp::Adapter::pEpLog::log(msg_.str());                                                 \
        } while (0)
#endif // TESTLOG

// TESTLOGH1 - logformat "Thread - __FILE__::__FUNTION__ - <=============== message ==============>"
#ifndef TESTLOGH1
    #define TESTLOGH1(msg)                                                                         \
        do {                                                                                       \
            std::stringstream msg_;                                                                \
            msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";                 \
            msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                     \
            msg_ << " - " << pEp::Adapter::pEpLog::decorateH1(msg);                                \
            pEp::Adapter::pEpLog::log(msg_.str());                                                 \
        } while (0)
#endif // TESTLOGH1

// TESTLOGH2 - logformat "Thread - __FILE__::__FUNTION__ - <--------------- message -------------->"
#ifndef TESTLOGH2
    #define TESTLOGH2(msg)                                                                         \
        do {                                                                                       \
            std::stringstream msg_;                                                                \
            msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";                 \
            msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                     \
            msg_ << " - " << pEp::Adapter::pEpLog::decorateH2(msg);                                \
            pEp::Adapter::pEpLog::log(msg_.str());                                                 \
        } while (0)
#endif // TESTLOGH2

// ------------------------------------------------------------------------------------------------


#endif // LIBPEPADAPTER_TEST_UTILS_HH