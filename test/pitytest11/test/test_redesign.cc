#include "../src/PityUnit.hh"
#include "../src/PityModel.hh"
#include "../src/PitySwarm.hh"

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;

void test_node1(PityUnit<PityModel>& unit,PityModel* model, PityPerspective* psp)
{
    unit.log("ModelName:" + model->getName());
    unit.log("perspective name:" + psp->name);
    unit.log("perspective partner:" + psp->partner);
    unit.log("perspective peers:\n" + Utils::to_string(psp->peers));

    std::string msg = "Message from: " + unit.getPathShort();
    int throttle = 1000;
    while (true) {
        Utils::sleep_millis(throttle);
        for (auto peer : unit.transportEndpoints()) {
            unit.log("sending to:" + peer.first);
            unit.transport()->sendMsg(peer.first,msg);
            Utils::sleep_millis(throttle);
        }
//
//        while (model->hasMsg()) {
//            unit.log("MSG RX:" + model->receiveMsg());
//            Utils::sleep_millis(throttle);
//        }
//    }
}

int main(int argc, char* argv[])
{
    // debug log per class
    PityModel::debug_log_enabled = false;
    PityNode::debug_log_enabled = false;
    PityUnit<>::debug_log_enabled = false;

    // Create model with 3 nodes
    PityModel model{ "test_redesign", 3 };

    PitySwarm swarm{model};

    swarm.addTestUnit(0,"test1",&test_node1);
    swarm.addTestUnit(1,"test1",&test_node1);
    swarm.addTestUnit(2,"test1",&test_node1);

    swarm.run();
}