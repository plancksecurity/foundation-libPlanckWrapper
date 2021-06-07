#include "../src/PityUnit.hh"
#include "../src/PityModel.hh"
#include "../../framework/utils.hh"
#include "../../../src/std_utils.hh"
#include "../../../src/pEpLog.hh"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;


void test_node1(const PityUnit<PityModel>& unit) {
}


int main(int argc, char* argv[])
{
    pEpLog::log("FSDFSD");
    PityModel::debug_log_enabled = false;
    PityNode::debug_log_enabled = false;
    PityModel model{ "test_Model", 3 };

    for (PityNode n : model.getNodes()) {
        pEpLog::log(n.to_string());
    }

    auto node1_unit = model.getNodes().at(0).getProcessUnit();
    PityUnit<PityModel> node1_test1 = PityUnit<PityModel>{ node1_unit.get(), "test1", nullptr };
//    model.getPerspective(0);

    model.rootUnit().run();
    //    pEpLog::log(model.rootUnit().to_string());
}