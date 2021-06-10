#include "../src/PityUnit.hh"
#include "../src/PityModel.hh"
#include "../src/PitySwarm.hh"
#include "../src/PityPerspective.hh"

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;

void test_node1(PityUnit<PityPerspective>& unit, PityPerspective* psp)
{
    unit.log("ModelName:" + psp->model.getName());
    unit.log("perspective name:" + psp->name);
    unit.log("perspective partner:" + psp->partner);

    std::string msg = "Message from: " + unit.getPathShort();
    int throttle = 1000;
    while (true) {
        Utils::sleep_millis(throttle);
        for (auto peer : unit.transportEndpoints()) {
            unit.transport()->sendMsg(peer.first,msg);
        }

        while (unit.transport()->hasMsg()) {
            unit.log("MSG RX:" + unit.transport()->receiveMsg());
        }
    }
}

int main(int argc, char* argv[])
{
    int nodesCount = 23;
    PityModel model{ "test_swarm", nodesCount };
    PitySwarm swarm{model};

    for(int i = 0; i < nodesCount; i++) {
        swarm.addTestUnit(i,"test1",&test_node1);
    }

    swarm.run();
}