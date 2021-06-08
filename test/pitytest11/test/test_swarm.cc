#include "../src/PityUnit.hh"
#include "../src/PityModel.hh"
#include "../../../src/std_utils.hh"

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;

void test_node1(const PityUnit<PityModel>& unit)
{
    unit.log("ModelName:" + unit.getModel()->getName());
    unit.log("own_node:" + unit.getModel()->own_node->getName());
    unit.log("partner:" + unit.getModel()->own_node->partner);

    std::string msg = "Message from: " + unit.getPathShort();
    int throttle = 10;
    while (true) {
        Utils::sleep_millis(throttle);
        for (auto peer : unit.getModel()->own_node->peers) {
            unit.getModel()->sendMsg(peer, msg);
            Utils::sleep_millis(throttle);
        }

        while (unit.getModel()->hasMsg()) {
            unit.log("MSG RX:" + unit.getModel()->receiveMsg());
            Utils::sleep_millis(throttle);
        }
    }
}

int main(int argc, char* argv[])
{
    int nodesCount = 64;
    PityModel model{ "test_swarm", nodesCount };

    std::vector<std::shared_ptr<PityUnit<PityModel>>> nodeUnits;
    for (int i = 0; i < nodesCount; i++) {
        nodeUnits.emplace_back(std::make_shared<PityUnit<PityModel>>(model.unitOfNodeNr(i), "test1", &test_node1 ));
    }

    model.run();
}