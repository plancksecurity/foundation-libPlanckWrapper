#include "../src/PityUnit.hh"
#include "../src/PityModel.hh"
#include "../src/PitySwarm.hh"
#include "../src/PityPerspective.hh"

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;

int test_node1(PityUnit<PityPerspective>& unit, PityPerspective* ctx)
{
    unit.log("ModelName:" + ctx->model.getName());
    unit.log("perspective name:" + ctx->own_name);
    unit.log("perspective partner:" + ctx->cpt_name);
    unit.log("HOME: " + std::string(getenv("HOME")));

    std::string msg = "Message from: " + unit.getPathShort();
    int throttle = 1000;
    while (true) {
        Utils::sleep_millis(throttle);
        for (const auto& peer : ctx->peers) {
            unit.transport()->sendMsg(peer, msg);
        }

        while (unit.transport()->hasMsg()) {
            unit.log("MSG RX:" + unit.transport()->receiveMsg());
        }
    }
}

int main(int argc, char* argv[])
{
    int nodesCount = 3;
    PityModel model{ "test_swarm", nodesCount };
    PitySwarm swarm{ model };

    for (int i = 0; i < nodesCount; i++) {
        swarm.addTestUnit(i, "test1", &test_node1);
    }

    swarm.run();
}