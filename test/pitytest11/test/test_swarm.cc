#include "../src/PityTest.hh"
#include "../../../src/utils.hh"
using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;

using TextCTX = PityModel;
using TestUnit = PityUnit<TextCTX>;

using TextCTXSwarm = PityPerspective;
using TestUnitSwarm = PityUnit<TextCTXSwarm>;

int test_init(PityUnit<PityPerspective>& unit, PityPerspective* ctx)
{
    unit.log("GlobalRoot:" + unit.getGlobalRootDir());
    unit.log("Path:" + unit.getPath());
    unit.log("ProcessDir:" + unit.getProcessDir());
    unit.log("TransportDir:" + unit.getTransportDir());

    PITYASSERT(
        unit.getProcessDir() ==
            unit.getGlobalRootDir() +
                AbstractPityUnit::_normalizeName(unit.getParentProcessUnit().getPath()) + "/",
        "ProcessDir");
    PITYASSERT(std::string(getenv("HOME")) == unit.getProcessDir(), "HOME");
    PITYASSERT(unit.getTransportDir() == unit.getProcessDir() + "inbox/", "TransportDir");
    return 0;
}

int test_run(PityUnit<PityPerspective>& unit, PityPerspective* ctx)
{
    std::string msg = "Message from: " + unit.getPath();
    int throttle = 1000;
    int cycles = 3;
    for (int i = 0; i < cycles; i++) {
        Utils::sleep_millis(throttle);
        unit.log(std::to_string(ctx->peers.size()));
        for (const auto& peer : ctx->peers) {
            unit.log("sending to" + peer);
            unit.transport()->sendMsg(peer, msg);
        }

        while (unit.transport()->hasMsg()) {
            unit.log(unit.getPath() + " - MSG RX:" + unit.transport()->receiveMsg());
        }
    }
    return 0;
}

int test_finish(PityUnit<PityPerspective>& unit, PityPerspective* ctx)
{
    unit.log(unit.getPath() + " - DONE");
    return 0;
}

int main(int argc, char* argv[])
{
    int nodesCount = 3;
    PityModel model{ "model_swarm", nodesCount };
    TestUnit suite{ "suite_swarm" };

    PitySwarm swarm1{ "swarm1", model };
    for (int i = 0; i < nodesCount; i++) {
        swarm1.addTestUnit(i, TestUnitSwarm("init", &test_init));
        swarm1.addTestUnit(i, TestUnitSwarm("run", &test_run));
    }
    std::cout << swarm1.getSwarmUnit().to_string() << std::endl;

    // swarm2 copy of swarm1
    PitySwarm swarm2{ swarm1, "swarm2" };
    // modify
    for (int i = 0; i < nodesCount; i++) {
        swarm2.addTestUnit(i, TestUnitSwarm("finish", &test_finish));
    }
    //    swarm2.getSwarmUnit().getChildRefs().begin()->second.setName("FDAGAFG");
    //    swarm2.getSwarmUnit().getChildRefs().begin()->second.getChildRefs().begin()->second.setName("fsadAG");
    std::cout << swarm1.getSwarmUnit().to_string() << std::endl;
    std::cout << swarm2.getSwarmUnit().to_string() << std::endl;

    suite.addRef(swarm1.getSwarmUnit());
    //    TODO this is broken, will not be run
    suite.addRef(swarm2.getSwarmUnit());
    suite.run();

    //    swarm1.run();
    //    Utils::readKey();
    //    swarm2.run();
}