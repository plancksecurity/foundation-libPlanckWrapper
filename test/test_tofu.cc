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


void tofu(PityUnit<PityPerspective> &unit, PityPerspective *ctx, bool init)
{
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
    if (init) {
        unit.log("Initiating TOFU...");
        pEpMessage msg = createMessage(my_ident, ctx->partner, "INIT TOFU");
        std::string mime_data = encryptAndEncode(msg);
        unit.transport()->sendMsg(ctx->partner, mime_data);
    }

    while (true) {
        std::string mime_data_rx = unit.transport()->receiveMsg();
        pEpMessage msg_rx = decryptAndDecode(mime_data_rx);
        unit.log(">");
        //
        pEpIdent rx_from = wrap(msg_rx->from);
        std::string rx_longmessage = msg_rx->longmsg;
        //
        pEpMessage msg = createMessage(
            my_ident,
            rx_from->address,
            "REPLY[ " + std::string(rx_from->address) + " ] " + rx_longmessage);
        std::string mime_data_tx = encryptAndEncode(msg);
        unit.transport()->sendMsg(rx_from->address, mime_data_tx);

        unit.log("<");
        Utils::sleep_millis(1000);
    }
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