// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_TEST_UTILS_HH
#define LIBPEPADAPTER_TEST_UTILS_HH

#include "../../src/pEpLog.hh"
#include <string>
#include <exception>
#include <chrono>
#include <thread>
#include <cstring>
#include <tuple>
#include <pEp/pEpEngine.h>
#include <pEp/identity_list.h>
#include <pEp/message.h>
#include <pEp/message_api.h>

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
            msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";                   \
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
            msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";                   \
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
            msg_ << "[" << getpid() << " " << std::this_thread::get_id() << "]";                   \
            msg_ << " - " << __FILE__ << "::" << __FUNCTION__;                                     \
            msg_ << " - " << pEp::Adapter::pEpLog::decorateH2(msg);                                \
            pEp::Adapter::pEpLog::log(msg_.str());                                                 \
        } while (0)
#endif // TESTLOGH2

// ------------------------------------------------------------------------------------------------

namespace pEp {
    namespace Test {
        namespace Utils {
            using pEpIdent = std::shared_ptr<::pEp_identity>;
            using pEpIdentList = std::shared_ptr<::identity_list>;
            using pEpMessage = std::shared_ptr<::message>;
            // [ DecryptedMessage, Rating, KeyList, Flags, WasEncrypted ]
            using DecryptResult = std::
                tuple<pEpMessage, ::PEP_rating, ::stringlist_t *, ::PEP_decrypt_flags_t *, bool>;
            // [ EncryptedMessage, MimeText, couldEncrypt ]
            using EncryptResult = std::tuple<pEpMessage, std::string, bool>;

            // Datatypes
            //Ident
            pEpIdent wrap(::pEp_identity *const ident);
            pEpIdent appropriate(::pEp_identity *const ident);
            pEpIdent dup(const ::pEp_identity *const ident);
            pEpIdent kill(::pEp_identity *const ident);

            //IdentityList
            pEpIdentList wrap(::identity_list *const ident);
            pEpIdentList appropriate(::identity_list *const ident);
            pEpIdentList dup(const ::identity_list *const ident);
            pEpIdentList kill(::identity_list *const ident);

            //Message
            pEpMessage wrap(::message *const msg);
            pEpMessage appropriate(::message *const msg);
            pEpMessage dup(const ::message *const msg);
            pEpMessage kill(::message *const msg);

            // Group
            struct Group {
                std::string name;
                std::string moderator;
                std::vector<std::string> members;
            };

            // helpers
            pEpIdent createOwnIdent(const std::string &address);
            pEpIdent createCptIdent(const std::string &address);
            pEpIdent createRawIdent(const std::string &address);
            pEpIdentList createIdentityList(const std::vector<std::string> &addresses);
            pEpMessage createMessage(pEpIdent from, pEpIdent to, const std::string &longmsg);
            pEpMessage createMessage(pEpIdent from, const std::string &to_addr, const std::string &longmsg);


            std::string mimeEncode(const pEpMessage msg);
            pEpMessage mimeDecode(const std::string &mime_text);

            EncryptResult encryptMessage(const pEpMessage msg);
            DecryptResult decryptMessage(const pEpMessage msg, ::PEP_decrypt_flags_t *flags);
            DecryptResult decryptMessage(const pEpMessage msg);

            EncryptResult encryptAndEncode(const pEpMessage msg);
            DecryptResult decryptAndDecode(const std::string &mime_data);

        } // namespace Utils
    }     // namespace Test
} // namespace pEp

#endif // LIBPEPADAPTER_TEST_UTILS_HH