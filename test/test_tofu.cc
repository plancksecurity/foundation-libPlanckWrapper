#include "pitytest11/src/PityUnit.hh"
#include "pitytest11/src/PityModel.hh"
#include "pitytest11/src/PitySwarm.hh"
#include "pitytest11/src/PityPerspective.hh"
#include "../src/utils.hh"
#include "framework/framework.hh"

#include <pEp/pEpEngine.h>
#include <pEp/message_api.h>
#include <pEp/keymanagement.h>
#include <pEp/identity_list.h>
#include <pEp/Adapter.hh>
#include <pEp/status_to_string.hh>
#include <pEp/mime.h>

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;

using pEpIdent = std::shared_ptr<::pEp_identity>;
using pEpMessage = std::shared_ptr<::message>;

//Ident
pEpIdent wrap(::pEp_identity *const ident) {
    assert(ident);
    auto ret = pEpIdent(ident, [](::pEp_identity *) {});
    return ret;
}

pEpIdent appropriate(::pEp_identity *const ident) {
    assert(ident);
    auto ret = pEpIdent(ident, ::free_identity);
    return ret;
}

pEpIdent dup(const ::pEp_identity *const ident) {
    assert(ident);
    auto ret = pEpIdent(::identity_dup(ident), ::free_identity);
    return ret;
}

pEpIdent kill(::pEp_identity *const ident) {
    assert(ident);
    auto ret = pEpIdent(::identity_dup(ident), ::free_identity);
    ::free_identity(ident);
    return ret;
}

//Message
pEpMessage wrap(::message *const msg) {
    assert(msg);
    auto ret = pEpMessage(msg, [](::message *) {});
    return ret;
}

pEpMessage appropriate(::message *const msg) {
    assert(msg);
    auto ret = pEpMessage(msg, ::free_message);
    return ret;
}

pEpMessage dup(const ::message *const msg) {
    assert(msg);
    auto ret = pEpMessage(::message_dup(msg), ::free_message);
    return ret;
}

pEpMessage kill(::message *const msg) {
    assert(msg);
    auto ret = pEpMessage(::message_dup(msg), ::free_message);
    ::free_message(msg);
    return ret;
}


pEpIdent createIdentity(const std::string &address, bool myself) {
    std::string name;
    if (myself) {
        name = "myself " + address;
    } else {
        name = "partner " + address;
    }
    ::pEp_identity *partner = ::new_identity(strdup(address.c_str()), NULL, PEP_OWN_USERID,
                                             strdup(name.c_str()));
    return appropriate(partner);
}


pEpMessage createMessage(pEpIdent from, pEpIdent to, const std::string &longmsg) {
    // create and fill in msg
    ::message *msg = ::new_message(PEP_dir_outgoing);
    msg->from = ::identity_dup(from.get());
    msg->to = ::new_identity_list(::identity_dup(to.get()));;
    msg->longmsg = strdup(longmsg.c_str());

    pEpMessage ret = appropriate(msg);
    return ret;
}

pEpMessage createMessage(pEpIdent from, const std::string &to_addr, const std::string &longmsg) {
    auto to_ident = createIdentity(to_addr, false);
    return createMessage(from, to_ident, longmsg);
}

pEpMessage mimeDecode(const std::string &mime_text) {
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

std::string mimeEncode(pEpMessage msg) {
    char *mimetext;
    PEP_STATUS status = ::mime_encode_message(msg.get(), false, &mimetext, false);
    throw_status(status);
    std::string text{mimetext};
    free(mimetext);
    return text;
}

void encryptAndSend(pEpIdent from, std::string to_addr, std::string longmsg, PityUnit<PityPerspective> &unit) {
    pEpMessage msg = createMessage(from, to_addr, "INIT_TOFU");
    ::message *msgenc = nullptr;
    PEP_STATUS status = ::encrypt_message(Adapter::session(), msg.get(), nullptr, &msgenc, PEP_enc_PEP, 0);
    throw_status(status);
    unit.log("encryptAndSend: " + status_to_string(status));
    unit.log("encryptAndSend" + Utils::to_string(msgenc, false));
    std::string mime_text = mimeEncode(wrap(msgenc));
    unit.transport()->sendMsg(to_addr, mime_text);
}

pEpMessage receiveAndDecrypt(PityUnit<PityPerspective> &unit) {
    pEpMessage ret;
    std::string rx_data = unit.transport()->receiveMsg();
    pEpMessage rx_msg = mimeDecode(rx_data);

    ::message *dec{nullptr};
    ::stringlist_t *kl = ::new_stringlist("");
    PEP_rating rating;
    unsigned int flags{0};
    PEP_STATUS status = ::decrypt_message(Adapter::session(), rx_msg.get(), &dec, &kl, &rating,
                                          &flags);
    throw_status(status);
    if (dec != nullptr) {
        ret = appropriate(dec);
    } else {
        unit.log("NOT DECRYPTED");
        ret = rx_msg;
    }
    unit.log("receiveAndDecrypt" + status_to_string(status));
    unit.log("receiveAndDecrypt" + Utils::to_string(ret.get(), false));
    return ret;
}

void tofu(PityUnit<PityPerspective> &unit, PityPerspective *ctx, bool init) {
    unit.log("Model  : " + ctx->model.getName());
    unit.log("myself : " + ctx->name);
    unit.log("partner: " + ctx->partner);
    unit.log("HOME   : " + std::string(getenv("HOME")));
    unit.log("PUD    : " + std::string(::per_user_directory()));

    // Create new identity
    unit.log("updating or creating identity for me");
    pEpIdent my_ident = createIdentity(ctx->name, true);
    ::PEP_STATUS status = ::myself(Adapter::session(), my_ident.get());
    pEp::throw_status(status);
//    unit.log(status_to_string(status));
//    unit.log(Utils::to_string(my_ident.get()));
    // ACT
    if (init) {
        unit.log("Initiating TOFU...");
        encryptAndSend(my_ident, ctx->partner, "INIT TOFU", unit);
    }

//    Utils::readKey();
    while (true) {
        pEpMessage msg_rx = receiveAndDecrypt(unit);
        unit.log(">");
        //
        pEpIdent rx_from = wrap(msg_rx->from);
        std::string rx_longmessage = msg_rx->longmsg;
        //
        encryptAndSend(my_ident, rx_from->address,
                       "REPLY[ " + std::string(rx_from->address) + " ] " + rx_longmessage, unit);
        unit.log("<");
        Utils::sleep_millis(10000);
    }
}


int main(int argc, char *argv[]) {
    PityUnit<PityPerspective>::debug_log_enabled = false;

    int nodesCount = 2;
    PityModel model{"test_tofu", nodesCount};
    PitySwarm swarm{model};

    swarm.addTestUnit(0, "tofu1", [](PityUnit<PityPerspective> &unit, PityPerspective *ctx) {
        tofu(unit, ctx, true);
    });
    swarm.addTestUnit(1, "tofu2", [](PityUnit<PityPerspective> &unit, PityPerspective *ctx) {
        tofu(unit, ctx, false);
    });

    swarm.run();
}