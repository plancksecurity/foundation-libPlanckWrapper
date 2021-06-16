#include "pitytest11/src/PityUnit.hh"
#include "pitytest11/src/PityModel.hh"
#include "pitytest11/src/PitySwarm.hh"
#include "pitytest11/src/PityPerspective.hh"
#include "../src/utils.hh"
#include "framework/framework.hh"
#include "framework/utils.hh"

#include <pEp/pEpEngine.h>
#include <pEp/message_api.h>
#include <pEp/keymanagement.h>
#include <pEp/identity_list.h>
#include <pEp/Adapter.hh>
#include <pEp/status_to_string.hh>
#include <pEp/mime.h>

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::Test::Utils;
using namespace pEp::PityTest11;

bool did_tx_encrypted = false;
bool did_rx_encrypted = false;

// The most minimal received msg contains of:
// * from addres
// * content
using MinMsgRx =std::tuple<std::string, std::string>;

void send(PityUnit<PityPerspective> &pity, PityPerspective *ctx)
{
    pity.log("Initiating TOFU...");
    pEpMessage msg = createMessage(ctx->own_ident, ctx->cpt_name, "INIT TOFU");
    EncryptResult msg_enc = encryptAndEncode(msg);
    std::string mime_text = std::get<1>(msg_enc);
    did_tx_encrypted = std::get<2>(msg_enc);
    pity.transport()->sendMsg(ctx->cpt_name, mime_text);
}

MinMsgRx receive(PityUnit<PityPerspective> &pity, PityPerspective *ctx)
{
    MinMsgRx ret;
    std::string mime_data_rx = pity.transport()->receiveMsg();
    DecryptResult msg_rx = decryptAndDecode(mime_data_rx);
    pEpMessage msg_rx_dec = std::get<0>(msg_rx);
    did_rx_encrypted = std::get<4>(msg_rx);
    pity.log("rx msg is encrypted: " + std::to_string(did_rx_encrypted));
    std::get<0>(ret) = std::string(msg_rx_dec->from->address);
    std::get<1>(ret) = std::string(msg_rx_dec->longmsg);
    return ret;
}

void reply(PityUnit<PityPerspective> &pity, PityPerspective *ctx, MinMsgRx msg_orig)
{
    std::string addr_orig = std::get<0>(msg_orig);
    std::string longmsg_orig = std::get<1>(msg_orig);

    pEpMessage msg = createMessage(
        ctx->own_ident,
        addr_orig,
        "REPLY[ " + std::string(addr_orig) + " ] " + longmsg_orig);
    EncryptResult eres = encryptAndEncode(msg);
    std::string mime_data_tx = std::get<1>(eres);
    did_tx_encrypted = std::get<2>(eres);
    pity.log("tx msg is encrypted: " + std::to_string(did_tx_encrypted));
    pity.transport()->sendMsg(addr_orig, mime_data_tx);
}

void tofu(PityUnit<PityPerspective> &pity, PityPerspective *ctx, bool init)
{
    pity.log("Model  : " + ctx->model.getName());
    pity.log("myself : " + ctx->own_name);
    pity.log("partner: " + ctx->cpt_name);
    pity.log("HOME   : " + std::string(getenv("HOME")));
    pity.log("PUD    : " + std::string(::per_user_directory()));


    // Create new identity
    pity.log("updating or creating identity for me");
    ctx->own_ident = createIdentity(ctx->own_name, true);
    ::PEP_STATUS status = ::myself(Adapter::session(), ctx->own_ident.get());
    pEp::throw_status(status);
    if (init) {
        send(pity, ctx);
    }

    MinMsgRx rx_msg = receive(pity,ctx);
    reply(pity,ctx, rx_msg);

    if(!init) {
        receive(pity,ctx);
    }

    PTASSERT(did_tx_encrypted, "could never send encrypted");
    PTASSERT(did_rx_encrypted, "no encrypted msg received");
}


int main(int argc, char *argv[])
{
    PityUnit<PityPerspective>::debug_log_enabled = false;

    int nodesCount = 2;
    PityModel model{ "test_tofu", nodesCount };
    PitySwarm swarm{ model };

    swarm.addTestUnit(0, "tofu1", [](PityUnit<PityPerspective> &unit, PityPerspective *ctx) {
        tofu(unit, ctx, true);
    });
    swarm.addTestUnit(1, "tofu2", [](PityUnit<PityPerspective> &unit, PityPerspective *ctx) {
        tofu(unit, ctx, false);
    });

    swarm.run();
}