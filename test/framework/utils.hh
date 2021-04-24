// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_UTILS_HH
#define LIBPEPADAPTER_UTILS_HH

#include "../../src/pEpLog.hh"
#include <string>
#include <pEp/message.h>
#include <pEp/identity_list.h>
#include <pEp/group.h>
#include <exception>

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
            msg_ << std::this_thread::get_id();                                                    \
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
            msg_ << std::this_thread::get_id();                                                    \
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
            msg_ << std::this_thread::get_id();                                                    \
            msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                     \
            msg_ << " - " << pEp::Adapter::pEpLog::decorateH2(msg);                                \
            pEp::Adapter::pEpLog::log(msg_.str());                                                 \
        } while (0)
#endif // TESTLOGH2

// ------------------------------------------------------------------------------------------------

namespace pEp {
    namespace Test {
        namespace Utils {
            // pEpEngine datatypes to string
            std::string to_string(const ::pEp_identity *const ident, bool full = true, int indent = 0);
            std::string to_string(const ::identity_list *const idl, bool full = true, int indent = 0);
            std::string to_string(const ::pEp_member *const member, bool full = true, int indent = 0);
            std::string to_string(const ::member_list *const mbl, bool full = true, int indent = 0);
            std::string to_string(const ::pEp_group *const group, bool full = true, int indent = 0);

            // C++/STL data types to string
            template<typename T>
            std::string to_string(const std::vector<T> &v);

            // exception utils
            std::string nested_exception_to_string(
                const std::exception &e,
                int level = 0,
                std::string src = "");
            void print_exception(const std::exception &e, int level = 0);

            // file utils
            std::ofstream file_create(const std::string &filename);
            bool file_exists(const std::string &filename);
            void file_delete(const std::string &filename);
            void file_ensure_not_existing(const std::string &path);
        } // namespace Utils
    }     // namespace Test
} // namespace pEp

#include "utils.hxx"

#endif // LIBPEPADAPTER_UTILS_HH