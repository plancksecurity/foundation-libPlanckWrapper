#include "../src/PityTest.hh"

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;

using TextCTX = PityPerspective;
using TestUnit = PityUnit<TextCTX>;

int test_init(PityUnit<PityPerspective>& unit, PityPerspective* ctx)
{
    unit.log("ModelName:" + ctx->model.getName());
    unit.log("perspective name:" + ctx->own_name);
    unit.log("perspective partner:" + ctx->cpt_name);
    unit.log("HOME: " + std::string(getenv("HOME")));
    return 0;
}

int test_run(PityUnit<PityPerspective>& unit, PityPerspective* ctx)
{
    std::string msg = "Message from: " + unit.getPathShort();
    int throttle = 10;
    int cycles = 10;
    for (int i = 0; i < cycles; i++) {
        Utils::sleep_millis(throttle);
        for (const auto& peer : ctx->peers) {
            unit.transport()->sendMsg(peer, msg);
        }

        while (unit.transport()->hasMsg()) {
            unit.log("MSG RX:" + unit.transport()->receiveMsg());
        }
    }
    return 0;
}

int test_finish(PityUnit<PityPerspective>& unit, PityPerspective* ctx)
{
    unit.log("DONE");
    return 0;
}

int main(int argc, char* argv[])
{
    int nodesCount = 3;
    PityModel model{ "model_swarm", nodesCount };
    PitySwarm swarm{ "swarm1", model };

    std::cout << swarm.getSwarmUnit().to_string() << std::endl;
    for (int i = 0; i < nodesCount; i++) {
        swarm.addTestUnit(i, TestUnit("test1", &test_init));
        swarm.addTestUnit(i, TestUnit("test1", &test_run));
        swarm.addTestUnit(i, TestUnit("test1", &test_finish));
    }

    swarm.run();
}