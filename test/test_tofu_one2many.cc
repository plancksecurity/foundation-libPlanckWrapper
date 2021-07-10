#include "pitytest11/src/PityTest.hh"
#include "../src/utils.hh"
#include "framework/framework.hh"
#include "framework/utils.hh"

// libpEpAdapter
#include "../src/Adapter.hh"
#include "../src/group_manager_api.h"
#include "../src/pEpLog.hh"
#include "../src/status_to_string.hh"
#include "../src/grp_driver_replicator.hh"

// engine
#include <pEp/pEpEngine.h>
#include <pEp/message_api.h>
#include <pEp/keymanagement.h>
#include <pEp/identity_list.h>
#include <pEp/mime.h>

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::Test::Utils;
using namespace pEp::PityTest11;
using namespace pEp::Adapter::pEpLog;

using TextContext = PityPerspective;
using TestUnitSwarm = PityUnit<TextContext>;

TextContext *local_ctx;
PityUnit<TextContext> *local_pity;

// The most minimal received msg contains of:
// * from address
// * content
using MinMsgRx = std::tuple<std::string, std::string>;

void tofu_send(TestUnitSwarm &pity, TextContext *ctx, const std::string &addr, const std::string longmessage)
{
    pEpMessage msg = createMessage(ctx->own_ident, addr, longmessage);
    EncryptResult msg_enc = encryptAndEncode(msg);

    std::string mime_text = std::get<1>(msg_enc);
    ctx->getPeer(addr)->did_tx_encrypted = std::get<2>(msg_enc);
    pity.log("Sending Encrypted[" + std::to_string(std::get<2>(msg_enc)) + "] to: " + addr);
    pity.transport()->sendMsg(addr, mime_text);
}

MinMsgRx tofu_receive(TestUnitSwarm &pity, TextContext *ctx)
{
    MinMsgRx ret;
    const std::string mime_data_rx = pity.transport()->receiveMsg();

    DecryptResult msg_rx = decryptAndDecode(mime_data_rx);
    pEpMessage msg_rx_dec = std::get<0>(msg_rx);
    ctx->getPeer(msg_rx_dec->from->address)->did_rx_encrypted = std::get<4>(msg_rx);
    pity.log(
        "Received Encrypted[" + std::to_string(std::get<4>(msg_rx)) +
        "] from: " + std::string(msg_rx_dec->from->address));
    //    pity.log("IN:\n " + Utils::to_string(mimeDecode(mime_data_rx).get(), false));
    pity.log("DECRYPTED:\n " + Utils::to_string(msg_rx_dec.get(), false));
    std::get<0>(ret) = std::string(msg_rx_dec->from->address);
    std::get<1>(ret) = std::string(msg_rx_dec->longmsg);
    return ret;
}

void tofu_receiveAndReply(TestUnitSwarm &pity, TextContext *ctx)
{
    MinMsgRx rx_msg = tofu_receive(pity, ctx);

    std::string addr = std::get<0>(rx_msg);
    std::string longmsg_orig = std::get<1>(rx_msg);
    pEpMessage msg = createMessage(
        ctx->own_ident,
        addr,
        "REPLY_FROM:'" + std::string(addr) + "' - " + longmsg_orig);

    tofu_send(pity, ctx, addr, "REPLY_FROM:'" + std::string(addr) + "' - " + longmsg_orig);
}

// TESTUNITS
// ------------------------------------------------------------------------------------------------

int test_create_myself(TestUnitSwarm &pity, TextContext *ctx)
{
    // Create new identity
    pity.log("updating or creating identity for me");
    ctx->own_ident = createOwnIdent(ctx->own_name);
    ::PEP_STATUS status = ::myself(Adapter::session(), ctx->own_ident.get());
    // Create cpt
    //    PITYASSERT(ctx->cpt != nullptr, "");
    //    ctx->cpt->ident = createCptIdent(ctx->getCpt().addr);
    pEp::throw_status(status);
    return 0;
}

int test_tofu_init_all_peers(TestUnitSwarm &pity, TextContext *ctx)
{
    for (auto &peer : ctx->peers) {
        tofu_send(pity, ctx, peer.addr, "INIT TOFU");
        tofu_receiveAndReply(pity, ctx);
        PITYASSERT(peer.tofu_done(), "TOFU failed for " + peer.addr);
    }
    return 0;
}

int test_tofu_react(TestUnitSwarm &pity, TextContext *ctx)
{
    tofu_receiveAndReply(pity, ctx);
    tofu_receive(pity, ctx);
    PITYASSERT(ctx->getCpt().tofu_done(), "TOFU failed for" + ctx->getCpt().addr);
    return 0;
}


int main(int argc, char *argv[])
{
    //    Adapter::pEpLog::set_enabled(true);
    //    Adapter::GroupDriverReplicator::log_enabled = true;
    //    Adapter::GroupDriverEngine::log_enabled = true;
    //    Adapter::GroupDriverDummy::log_enabled = true;
    //    TestUnit::debug_log_enabled = false;
    //    PityTransport::debug_log_enabled = true;
    int nodesCount = 23;
    PityModel model{ "model_tofu2", nodesCount };
    TestUnitSwarm suite = TestUnitSwarm("suite_tofu2");
    PitySwarm swarm{ "swarm_tofu2", model };
    suite.addRef(swarm.getSwarmUnit());
    // ------------------------------------------------------------------------------------
    swarm.addTestUnit(0, TestUnitSwarm("test_create_myself", test_create_myself));
    swarm.addTestUnit(0, TestUnitSwarm("test_tofu_init_all_peers", test_tofu_init_all_peers));

    for (int i = 1; i < nodesCount; i++) {
        swarm.addTestUnit(i, TestUnitSwarm("test_create_myself", test_create_myself));
        swarm.addTestUnit(i, TestUnitSwarm("test_tofu_react", test_tofu_react));
    }
    suite.run();
}
