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

// Either group manager or group member, not both
pEpIdent group_ident;

bool grp_did_tx_encrypted = false;
bool grp_did_rx_encrypted = false;
bool signal_received = false;
// The most minimal received msg contains of:
// * from address
// * content
using MinMsgRx = std::tuple<std::string, std::string>;

// CALLBACKS
// ------------------------------------------------------------------------------------------------

::PEP_STATUS test_messageToSend(::message *_msg)
{
    local_pity->log("MESSAGE TO SEND:" + Utils::to_string(_msg, false));
    std::string mime_text = mimeEncode(wrap(_msg));
    local_pity->transport()->sendMsg(_msg->to->ident->address, mime_text);
    return PEP_STATUS_OK;
}

::PEP_STATUS test_notifyHandshake(::pEp_identity *me, ::pEp_identity *partner, sync_handshake_signal signal)
{
    local_pity->log("NOTFY_HANDSHAKE: signal: " + std::string(std::to_string(signal)));
    local_pity->log("NOTFY_HANDSHAKE: me:" + Utils::to_string(me, false));
    local_pity->log("NOTFY_HANDSHAKE: partner: " + Utils::to_string(partner, false));
    if (signal == ::SYNC_NOTIFY_GROUP_INVITATION) {
        signal_received = true;
        group_ident = dup(me);
        local_pity->log("SYNC_NOTIFY_GROUP_INVITATION");
        PEP_STATUS status = ::adapter_group_join(
            Adapter::session(),
            group_ident.get(),
            local_ctx->own_ident.get());
        throw_status(status);
    }
    return PEP_STATUS_OK;
}

// HELPERS
// ------------------------------------------------------------------------------------------------

// Blocking
void processMessage()
{
    local_pity->log("waiting for message...");
    std::string mime_data_rx = local_pity->transport()->receiveMsg();
    //        local_pity->log("mime_text:" + Utils::clip(mime_data_rx, 300));

    // Decode
    pEpMessage rx_msg = mimeDecode(mime_data_rx);
    //        local_pity->log("decode: " + Utils::to_string(rx_msg.get(), false));

    // Decrypt
    ::PEP_decrypt_flags_t flags = ::PEP_decrypt_flag_dont_trigger_sync;
    //        ::PEP_decrypt_flags_t flags = 0;
    DecryptResult decres = decryptMessage(rx_msg, &flags);
    pEpMessage msg_decrypt = std::get<0>(decres);
    //        local_pity->log("Decrypt: " + Utils::to_string(msg_decrypt.get(), false));
    local_pity->log("message processed...");
}

// Non-Blocking
void processsAllMessages()
{
    while (local_pity->transport()->hasMsg()) {
        Utils::sleep_millis(100);
        processMessage();
    }
}

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

int test_pEp_init(TestUnitSwarm &pity, TextContext *ctx)
{
    local_ctx = ctx;
    local_pity = &pity;
    pity.log("Model  : " + ctx->model.getName());
    pity.log("myself : " + ctx->own_name);
    pity.log("partner: " + ctx->getCpt().addr);
    pity.log("HOME   : " + std::string(getenv("HOME")));
    pity.log("PUD    : " + std::string(::per_user_directory()));
    pity.log("PMD    : " + std::string(::per_machine_directory()));
    pEp::Adapter::session.initialize(
        pEp::Adapter::SyncModes::Async,
        false,
        test_messageToSend,
        test_notifyHandshake);
    return 0;
}

int test_create_myself(TestUnitSwarm &pity, TextContext *ctx)
{
    // Create new identity
    pity.log("updating or creating identity for me");
    ctx->own_ident = createOwnIdent(ctx->own_name);
    ::PEP_STATUS status = ::myself(Adapter::session(), ctx->own_ident.get());
    pEp::throw_status(status);
    return 0;
}

int test_start_sync(TestUnitSwarm &pity, TextContext *ctx)
{
    Adapter::session
        .initialize(Adapter::SyncModes::Async, false, test_messageToSend, test_notifyHandshake);
    processsAllMessages();
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
    //    std::cout << pity.getParentProcessUnit().getName() << "GET:" << &ctx->getCpt() << std::endl;
    PITYASSERT(ctx->getCpt().tofu_done(), "TOFU failed for" + ctx->getCpt().addr);
    return 0;
}

int test_group_create(TestUnitSwarm &pity, TextContext *ctx)
{
    if (ctx->groups.size() > 0) {
        group_ident = createCptIdent(ctx->groups.front().addr);
        PEP_STATUS status = ::adapter_group_create(
            Adapter::session(),
            group_ident.get(),
            ctx->own_ident.get(),
            nullptr);
        pEp::throw_status(status);
    }
    return 0;
}

int test_group_invite_members(TestUnitSwarm &pity, TextContext *ctx)
{
    PEP_STATUS status;
    if (ctx->groups.size() > 0) {
        Group &my_grp = ctx->groups.at(0);
        auto grp_ident = createRawIdent(my_grp.addr);
        status = ::update_identity(Adapter::session(), grp_ident.get());
        throw_status(status);

        pity.log(Utils::to_string(grp_ident.get(), false));
        for (TestIdent mb : my_grp.members) {
            auto mb_ident = createRawIdent(mb.addr);
            status = ::update_identity(Adapter::session(), mb_ident.get());
            throw_status(status);
            pity.log(Utils::to_string(mb_ident.get(), false));

            status = ::adapter_group_invite_member(Adapter::session(), grp_ident.get(), mb_ident.get());
            throw_status(status);
        }
    }
    return 0;
}

int test_group_join(TestUnitSwarm &pity, TextContext *ctx)
{
    processMessage();
    PITYASSERT(signal_received, "test_group_join - no signal received");
    return 0;
}

int test_receive_joined(TestUnitSwarm &pity, TextContext *ctx)
{
    PEP_STATUS status;
    if (ctx->groups.size() > 0) {
        Group &my_grp = ctx->groups.at(0);
        for (TestIdent mb : my_grp.members) {
            processMessage();
        }
    }
    return 0;
}


int test_send_groupmessage(TestUnitSwarm &pity, TextContext *ctx)
{
    const std::string addr = group_ident->address;
    pEpMessage msg = createMessage(ctx->own_ident, addr, "GROUP MESSAGE");
    EncryptResult msg_enc = encryptAndEncode(msg);
    std::string mime_text = std::get<1>(msg_enc);
    grp_did_tx_encrypted = std::get<2>(msg_enc);
    pity.log("Sending to GROUP Encrypted[" + std::to_string(std::get<2>(msg_enc)) + "] to: " + addr);

    // Send to members and moderator (except self)
    for (const auto &member : ctx->getGroup(addr)->members) {
        if (member.addr != ctx->own_name) {
            pity.log("To: " + member.addr);
            pity.transport()->sendMsg(member.addr, mime_text);
        }
    }
    if (ctx->getGroup(addr)->moderator != ctx->own_name) {
        pity.log("To: " + ctx->getGroup(addr)->moderator);
        pity.transport()->sendMsg(ctx->getGroup(addr)->moderator, mime_text);
    }
    return 0;
}


int test_receive(TestUnitSwarm &pity, TextContext *ctx)
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
    //    pity.log("DECRYPTED:\n " + Utils::to_string(msg_rx_dec.get(), false));
    std::get<0>(ret) = std::string(msg_rx_dec->from->address);
    std::get<1>(ret) = std::string(msg_rx_dec->longmsg);
    return 0;
}

int test_receive_all(TestUnitSwarm &pity, TextContext *ctx)
{
    while (local_pity->transport()->hasMsg()) {
        Utils::sleep_millis(100);
        test_receive(pity, ctx);
    }
    return 0;
}

int test_group_remove_member_one_by_one(TestUnitSwarm &pity, TextContext *ctx)
{
    bool is_empty = false;
    do {
        ::identity_list *idl = nullptr;
        ::PEP_STATUS status = ::adapter_group_query_members(Adapter::session(), group_ident.get(), &idl);
        throw_status(status);
        pity.log(Utils::to_string(idl, false));
        if (idl->ident != nullptr) {
            status = ::adapter_group_remove_member(Adapter::session(), group_ident.get(), idl->ident);
            throw_status(status);
        } else {
            is_empty = true;
        }
    } while (!is_empty);

    return 0;
}

int test_group_query1(TestUnitSwarm &pity, TextContext *ctx)
{
    {
        ::identity_list *idl = nullptr;
        ::PEP_STATUS status = ::adapter_group_query_groups(Adapter::session(), &idl);
        throw_status(status);
        pity.log(Utils::to_string(idl, false));
        PITYASSERT(idl->ident == nullptr, "adapter_group_query_groups");
    }
    return 0;
}

int test_group_query2(TestUnitSwarm &pity, TextContext *ctx)
{
    {
        ::identity_list *idl = nullptr;
        ::PEP_STATUS status = ::adapter_group_query_groups(Adapter::session(), &idl);
        throw_status(status);
        //        pity.log(Utils::to_string(idl, false));
        PITYASSERT(idl->ident->address == ctx->groups.begin()->addr, "adapter_group_query_groups");
    }
    {
        pEpIdent mgr = createRawIdent("");
        ::pEp_identity *mgr_ptr = mgr.get();
        ::PEP_STATUS status = ::adapter_group_query_manager(
            Adapter::session(),
            group_ident.get(),
            &mgr_ptr);
        throw_status(status);
        //        pity.log(Utils::to_string(mgr_ptr, false));
        //        pity.log(Utils::to_string(ctx->own_ident.get(), false));
        PITYASSERT(
            *mgr_ptr->address == *ctx->own_ident->address,
            "adapter_group_query_manager - wrong manager");
    }
    {
        ::identity_list *idl = nullptr;
        ::PEP_STATUS status = ::adapter_group_query_members(Adapter::session(), group_ident.get(), &idl);
        throw_status(status);
        //        pity.log(Utils::to_string(idl, false));
        PITYASSERT(idl->ident == nullptr, "adapter_group_query_members");
    }
    return 0;
}


int test_group_query3(TestUnitSwarm &pity, TextContext *ctx)
{
    {
        ::identity_list *idl = nullptr;
        ::PEP_STATUS status = ::adapter_group_query_groups(Adapter::session(), &idl);
        throw_status(status);
        //        pity.log(Utils::to_string(idl, false));
        PITYASSERT(idl->ident->address == ctx->groups.begin()->addr, "adapter_group_query_groups");
    }
    {
        pEpIdent mgr = createRawIdent("");
        ::pEp_identity *mgr_ptr = mgr.get();
        ::PEP_STATUS status = ::adapter_group_query_manager(
            Adapter::session(),
            group_ident.get(),
            &mgr_ptr);
        throw_status(status);
        //        pity.log(Utils::to_string(mgr_ptr, false));
        //        pity.log(Utils::to_string(ctx->own_ident.get(), false));
        PITYASSERT(
            *mgr_ptr->address == *ctx->own_ident->address,
            "adapter_group_query_manager - wrong manager");
    }
    {
        ::identity_list *idl = nullptr;
        ::PEP_STATUS status = ::adapter_group_query_members(Adapter::session(), group_ident.get(), &idl);
        throw_status(status);
        pity.log(Utils::to_string(idl, false));
        //        PITYASSERT(idl->ident->address == ctx->getCpt().addr, "adapter_group_query_members");
    }
    return 0;
}

int test_group_query4(TestUnitSwarm &pity, TextContext *ctx)
{
    {
        ::identity_list *idl = nullptr;
        ::PEP_STATUS status = ::adapter_group_query_groups(Adapter::session(), &idl);
        throw_status(status);
        //        pity.log(Utils::to_string(idl, false));
        PITYASSERT(idl->ident->address == ctx->groups.begin()->addr, "adapter_group_query_groups");
    }
    {
        pEpIdent mgr = createRawIdent("");
        ::pEp_identity *mgr_ptr = mgr.get();
        ::PEP_STATUS status = ::adapter_group_query_manager(
            Adapter::session(),
            group_ident.get(),
            &mgr_ptr);
        throw_status(status);
        //        pity.log(Utils::to_string(mgr_ptr, false));
        //        pity.log(Utils::to_string(ctx->own_ident.get(), false));
        PITYASSERT(
            *mgr_ptr->address == *ctx->own_ident->address,
            "adapter_group_query_manager - wrong manager");
    }
    {
        ::identity_list *idl = nullptr;
        ::PEP_STATUS status = ::adapter_group_query_members(Adapter::session(), group_ident.get(), &idl);
        throw_status(status);
        //        pity.log(Utils::to_string(idl, false));
        //        PITYASSERT(idl->ident->address == ctx->getCpt().addr, "adapter_group_query_members");
    }
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
    swarm.addTestUnit(0, TestUnitSwarm("test_pEp_init", test_pEp_init));
    swarm.addTestUnit(0, TestUnitSwarm("test_create_myself", test_create_myself));
    swarm.addTestUnit(0, TestUnitSwarm("test_tofu_init_all_peers", test_tofu_init_all_peers));
    swarm.addTestUnit(0, TestUnitSwarm("test_start_sync", test_start_sync));
    swarm.addTestUnit(0, TestUnitSwarm("test_group_query1", test_group_query1));
    swarm.addTestUnit(0, TestUnitSwarm("test_group_create", test_group_create));
    swarm.addTestUnit(0, TestUnitSwarm("test_group_query2", test_group_query2));
    swarm.addTestUnit(0, TestUnitSwarm("test_group_invite_members", test_group_invite_members));
    swarm.addTestUnit(0, TestUnitSwarm("test_group_query3", test_group_query3));
    swarm.addTestUnit(0, TestUnitSwarm("test_receive_joined", test_receive_joined));
    swarm.addTestUnit(0, TestUnitSwarm("test_group_query3", test_group_query3));
    swarm.addTestUnit(0, TestUnitSwarm("test_send_groupmessage", test_send_groupmessage));
    swarm.addTestUnit(0, TestUnitSwarm("test_receive", test_receive));
    swarm.addTestUnit(0, TestUnitSwarm("test_receive_all", test_receive_all));
    swarm.addTestUnit(
        0,
        TestUnitSwarm("test_group_remove_member_one_by_one", test_group_remove_member_one_by_one));
    swarm.addTestUnit(0, TestUnitSwarm("test_group_query4", test_group_query4));

    //------------------------------------------------------------------------------------
    for (int i = 1; i < nodesCount; i++) {
        swarm.addTestUnit(i, TestUnitSwarm("test_pEp_init", test_pEp_init));
        swarm.addTestUnit(i, TestUnitSwarm("test_create_myself", test_create_myself));
        swarm.addTestUnit(i, TestUnitSwarm("test_tofu_react", test_tofu_react));
        swarm.addTestUnit(i, TestUnitSwarm("test_start_sync", test_start_sync));
        swarm.addTestUnit(i, TestUnitSwarm("test_group_join", test_group_join));
        swarm.addTestUnit(i, TestUnitSwarm("test_receive", test_receive));
        swarm.addTestUnit(i, TestUnitSwarm("test_send_groupmessage", test_send_groupmessage));
        swarm.addTestUnit(i, TestUnitSwarm("test_receive_all", test_receive_all));
    }

    suite.run();
}
