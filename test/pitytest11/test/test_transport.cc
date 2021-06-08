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
    unit.log("peers:\n" + Utils::to_string(unit.getModel()->own_node->peers));

    std::string msg = "Message from: " + unit.getPathShort();
    int throttle = 2000;
    while (true) {
        Utils::sleep_millis(throttle);
        for (auto peer : unit.getModel()->own_node->peers) {
            unit.log("sending to:" + peer);
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
    // debug log per class
    PityModel::debug_log_enabled = false;
    PityNode::debug_log_enabled = false;
    PityUnit<>::debug_log_enabled = false;

    // Create model with 3 nodes
    PityModel model{ "test_transport", 3 };

    //    //Configure model
    //    model.nodeNr(0)->partner = model.nodeNr(1)->getName();
    //    model.nodeNr(1)->partner = model.nodeNr(2)->getName();
    //    model.nodeNr(2)->partner = model.nodeNr(0)->getName();

    PityUnit<PityModel> node1_test1 = PityUnit<PityModel>{ model.unitOfNodeNr(0),
                                                           "test1",
                                                           &test_node1 };
    PityUnit<PityModel> node2_test1 = PityUnit<PityModel>{ model.unitOfNodeNr(1),
                                                           "test1",
                                                           &test_node1 };
    PityUnit<PityModel> node3_test1 = PityUnit<PityModel>{ model.unitOfNodeNr(2),
                                                           "test1",
                                                           &test_node1 };

    model.run();
}