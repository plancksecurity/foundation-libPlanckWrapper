#include <pEp/pitytest11/PityTest.hh>
#include "../src/utils.hh"
#include "framework/framework.hh"
#include <pEp/pitytest11/test_utils.hh>

#include <pEp/pEpEngine.h>
#include <pEp/keymanagement.h>
#include <pEp/Adapter.hh>
#include <pEp/status_to_string.hh>

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::Test::Utils;
using namespace pEp::PityTest11;

using TestUnitSwarm = PityUnit<PityPerspective>;


bool did_tx_encrypted = false;
bool did_rx_encrypted = false;

// The most minimal received msg contains of:
// * from addres
// * content
using MinMsgRx = std::tuple<std::string, std::string>;

void send(PityUnit<PityPerspective> &pity, PityPerspective *ctx)
{
    // Create Message
    pity.log("Initiating TOFU...");
    pEpMessage msg = createMessage(ctx->own_ident, ctx->getCpt().addr, "INIT TOFU");
    pity.log("BEFORE encrypt: \n" + Utils::to_string(msg.get()));

    //Encrypt
    EncryptResult msg_encrypted = encryptMessage(msg);
    did_tx_encrypted = std::get<2>(msg_encrypted);
    pity.log("TX COULD ENCRYPT: " + std::to_string(did_tx_encrypted));
    pity.log("AFTER encrypt: \n" + Utils::to_string(std::get<0>(msg_encrypted).get()));

    // Encode
    std::string mime_text = mimeEncode(std::get<0>(msg_encrypted));

    // Send
    pity.transport()->sendMsg(ctx->getCpt().addr, mime_text);
}

MinMsgRx tofu_receive(PityUnit<PityPerspective> &pity, PityPerspective *ctx)
{
    MinMsgRx ret;
    // Receive
    std::string mime_data_rx = pity.transport()->receiveMsg();

    // Decode
    pEpMessage mime_decoded = mimeDecode(mime_data_rx);
    pity.log("RX message - BEFORE decrypt: \n" + Utils::to_string(mime_decoded.get()));

    // Decrypt
    DecryptResult msg_decrypted = decryptMessage(mime_decoded);
    pEpMessage msg_rx_dec = std::get<0>(msg_decrypted);
    did_rx_encrypted = std::get<4>(msg_decrypted);
    pity.log("RX WAS ENCRYPTED: " + std::to_string(did_rx_encrypted));
    pity.log("RX message - AFTER decrypt: \n" + Utils::to_string(msg_rx_dec.get()));

    // Return result
    std::get<0>(ret) = std::string(msg_rx_dec->from->address);
    std::get<1>(ret) = std::string(msg_rx_dec->longmsg);
    return ret;
}

void receiveAndReply(PityUnit<PityPerspective> &pity, PityPerspective *ctx, MinMsgRx msg_orig)
{
    // Create Message
    std::string addr_orig = std::get<0>(msg_orig);
    std::string longmsg_orig = std::get<1>(msg_orig);

    pEpMessage msg = createMessage(
        ctx->own_ident,
        addr_orig,
        "REPLY[ " + std::string(addr_orig) + " ] " + longmsg_orig);

    // Encrypt
    pity.log("TX message - BEFORE encrypt: \n" + Utils::to_string(msg.get()));

    EncryptResult eres = encryptMessage(msg);
    pEpMessage msg_encrypted = std::get<0>(eres);
    did_tx_encrypted = std::get<2>(eres);
    pity.log("TX COULD ENCRYPT: " + std::to_string(did_tx_encrypted));
    pity.log("TX message - AFTER encrypt: \n" + Utils::to_string(msg_encrypted.get()));

    // Encode
    std::string mime_data_tx = mimeEncode(msg_encrypted);

    // Send
    pity.transport()->sendMsg(addr_orig, mime_data_tx);
}

int tofu(PityUnit<PityPerspective> &pity, PityPerspective *ctx, bool init)
{
    pity.log("Model  : " + ctx->model.getName());
    pity.log("myself : " + ctx->own_name);
    pity.log("partner: " + ctx->getCpt().addr);
    pity.log("HOME   : " + std::string(getenv("HOME")));
    pity.log("PUD    : " + std::string(::per_user_directory()));
    Adapter::session.initialize();

    // Create new identity
    pity.log("updating or creating identity for me");
    ctx->own_ident = createOwnIdent(ctx->own_name);
    ::PEP_STATUS status = ::myself(Adapter::session(), ctx->own_ident.get());
    pEp::throw_status(status);
    if (init) {
        send(pity, ctx);
    }

    MinMsgRx rx_msg = tofu_receive(pity, ctx);
    receiveAndReply(pity, ctx, rx_msg);

    if (!init) {
        tofu_receive(pity, ctx);
    }

    PITYASSERT(did_tx_encrypted, "could never tofu_send encrypted");
    PITYASSERT(did_rx_encrypted, "no encrypted msg received");
    return 0;
}


int main(int argc, char *argv[])
{
    PityUnit<PityPerspective>::debug_log_enabled = false;

    int nodesCount = 2;
    PityModel model{ "test_tofu_react", nodesCount };

    TestUnitSwarm suite("suite_tofu");
    PitySwarm swarm{ "swarm_tofu", model };
    suite.addRef(swarm.getSwarmUnit());

    swarm.addTestUnit(
        0,
        TestUnitSwarm("tofu1", [](PityUnit<PityPerspective> &unit, PityPerspective *ctx) {
            return tofu(unit, ctx, true);
        }));

    swarm.addTestUnit(
        1,
        TestUnitSwarm("tofu2", [](PityUnit<PityPerspective> &unit, PityPerspective *ctx) {
            return tofu(unit, ctx, false);
        }));

    suite.run();
}