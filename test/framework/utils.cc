#include "utils.hh"

#include <pEp/pEpEngine.h>
#include <pEp/message_api.h>
#include <pEp/keymanagement.h>
#include <pEp/identity_list.h>
#include <pEp/Adapter.hh>
#include <pEp/status_to_string.hh>
#include <pEp/mime.h>
#include <tuple>

namespace pEp {
    namespace Test {
        namespace Utils {

            //Ident
            pEpIdent wrap(::pEp_identity *const ident)
            {
                assert(ident);
                auto ret = pEpIdent(ident, [](::pEp_identity *) {});
                return ret;
            }

            pEpIdent appropriate(::pEp_identity *const ident)
            {
                assert(ident);
                auto ret = pEpIdent(ident, ::free_identity);
                return ret;
            }

            pEpIdent dup(const ::pEp_identity *const ident)
            {
                assert(ident);
                auto ret = pEpIdent(::identity_dup(ident), ::free_identity);
                return ret;
            }

            pEpIdent kill(::pEp_identity *const ident)
            {
                assert(ident);
                auto ret = pEpIdent(::identity_dup(ident), ::free_identity);
                ::free_identity(ident);
                return ret;
            }

            //IdentityList
            pEpIdentList wrap(::identity_list *const ident)
            {
                assert(ident);
                auto ret = pEpIdentList(ident, [](::identity_list *) {});
                return ret;
            }

            pEpIdentList appropriate(::identity_list *const ident)
            {
                assert(ident);
                auto ret = pEpIdentList(ident, ::free_identity_list);
                return ret;
            }

            pEpIdentList dup(const ::identity_list *const ident)
            {
                assert(ident);
                auto ret = pEpIdentList(::identity_list_dup(ident), ::free_identity_list);
                return ret;
            }

            pEpIdentList kill(::identity_list *const ident)
            {
                assert(ident);
                auto ret = pEpIdentList(::identity_list_dup(ident), ::free_identity_list);
                ::free_identity_list(ident);
                return ret;
            }

            //Message
            pEpMessage wrap(::message *const msg)
            {
                assert(msg);
                auto ret = pEpMessage(msg, [](::message *) {});
                return ret;
            }

            pEpMessage appropriate(::message *const msg)
            {
                assert(msg);
                auto ret = pEpMessage(msg, ::free_message);
                return ret;
            }

            pEpMessage dup(const ::message *const msg)
            {
                assert(msg);
                auto ret = pEpMessage(::message_dup(msg), ::free_message);
                return ret;
            }

            pEpMessage kill(::message *const msg)
            {
                assert(msg);
                auto ret = pEpMessage(::message_dup(msg), ::free_message);
                ::free_message(msg);
                return ret;
            }

            // helpers
            pEpIdent createOwnIdent(const std::string &address)
            {
                std::string name;
                ::pEp_identity *ident = nullptr;
                ident = ::new_identity(
                    strdup(address.c_str()),
                    "",
                    PEP_OWN_USERID,
                    ("myself " + address).c_str());
                ident->me = true;

                return appropriate(ident);
            }

            pEpIdent createCptIdent(const std::string &address)
            {
                std::string name;
                ::pEp_identity *ident = nullptr;
                ident = ::new_identity(
                    strdup(address.c_str()),
                    "",
                    "23",
                    ("partner " + address).c_str());
                ident->me = false;

                return appropriate(ident);
            }

            pEpIdent createRawIdent(const std::string &address)
            {
                std::string name;
                ::pEp_identity *ident = nullptr;
                ident = ::new_identity(
                    strdup(address.c_str()),
                    "",
                    "",
                    "");
                ident->me = false;

                return appropriate(ident);
            }

            pEpIdentList createIdentityList(const std::vector<std::string> &addresses)
            {
                ::identity_list *list;
                list = ::new_identity_list(nullptr);
                for (std::string addr : addresses) {
                    ::identity_list_add(list, ::identity_dup(createCptIdent(addr).get()));
                }
                return appropriate(list);
            }

            pEpMessage createMessage(pEpIdent from, pEpIdent to, const std::string &longmsg)
            {
                // create and fill in msg
                ::message *msg = ::new_message(PEP_dir_outgoing);
                msg->from = ::identity_dup(from.get());
                msg->to = ::new_identity_list(::identity_dup(to.get()));
                msg->longmsg = strdup(longmsg.c_str());

                pEpMessage ret = appropriate(msg);
                return ret;
            }

            pEpMessage createMessage(pEpIdent from, const std::string &to_addr, const std::string &longmsg)
            {
                pEpIdent to_ident = createCptIdent(to_addr);
                return createMessage(from, to_ident, longmsg);
            }

            std::string mimeEncode(const pEpMessage msg)
            {
                char *mimetext;
                PEP_STATUS status = ::mime_encode_message(msg.get(), false, &mimetext, false);
                throw_status(status);
                std::string text{ mimetext };
                free(mimetext);
                return text;
            }

            pEpMessage mimeDecode(const std::string &mime_text)
            {
                ::message *msg;
                bool has_possible_pEp_msg;
                ::PEP_STATUS status = ::mime_decode_message(
                    mime_text.c_str(),
                    mime_text.length(),
                    &msg,
                    &has_possible_pEp_msg);
                throw_status(status);
                return pEpMessage(msg, ::free_message);
            }

            EncryptResult encryptMessage(const pEpMessage msg)
            {
                pEpMessage msg_out;
                bool could_encrypt = false;
                ::message *msgenc = nullptr;
                PEP_STATUS status = ::encrypt_message(
                    Adapter::session(),
                    msg.get(),
                    nullptr,
                    &msgenc,
                    PEP_enc_PEP,
                    0);
                throw_status(status);
                ::message *msg_out_p = nullptr;
                if (msgenc != nullptr) {
                    could_encrypt = true;
                    msg_out = appropriate(msgenc);
                } else {
                    could_encrypt = false;
                    msg_out = msg;
                }
                return EncryptResult(msg_out, "", could_encrypt);
            }

            DecryptResult decryptMessage(const pEpMessage msg)
            {
                pEpMessage msg_out;
                bool was_encrypted = false;

                ::message *dec{ nullptr };
                ::stringlist_t *kl = ::new_stringlist("");
                ::PEP_rating rating;
                unsigned int flags{ 0 };
                PEP_STATUS status = ::decrypt_message(
                    Adapter::session(),
                    msg.get(),
                    &dec,
                    &kl,
                    &rating,
                    &flags);
                throw_status(status);
                if (dec != nullptr) {
                    was_encrypted = true;
                    msg_out = appropriate(dec);
                } else {
                    was_encrypted = false;
                    msg_out = msg;
                }
                return DecryptResult(msg_out, rating, kl, flags, was_encrypted);
            }

            EncryptResult encryptAndEncode(const pEpMessage msg)
            {
                EncryptResult ret = encryptMessage(msg);
                std::string mime_text = mimeEncode(std::get<0>(ret));
                std::get<1>(ret) = mime_text;
                return ret;
            }

            DecryptResult decryptAndDecode(const std::string &mime_data)
            {
                DecryptResult ret;
                pEpMessage rx_msg = mimeDecode(mime_data);
                ret = decryptMessage(rx_msg);
                return ret;
            }
        } // namespace Utils
    }     // namespace Test
} // namespace pEp