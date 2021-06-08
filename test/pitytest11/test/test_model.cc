#include "../src/PityUnit.hh"
#include "../src/PityModel.hh"
#include "../../framework/utils.hh"
#include "../../../src/std_utils.hh"
#include "../../../src/pEpLog.hh"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace pEp::Adapter;
using namespace pEp::PityTest11;


void test_node1(const PityUnit<PityModel>& unit)
{
    unit.log(unit.getModel()->getName());
    unit.log(unit.getModel()->own_node->);
}


int main(int argc, char* argv[])
{
    PityModel::debug_log_enabled = false;
    PityNode::debug_log_enabled = true;
    PityModel model{ "test_model", 3 };

    for (auto n : model.getNodes()) {
        pEpLog::log(n->getName());
    }

    PityUnit<PityModel> node1_test1 = PityUnit<PityModel>{ model.getNodeUnit(0), "test1", &test_node1 };
    PityUnit<PityModel> node2_test1 = PityUnit<PityModel>{ model.getNodeUnit(1), "test2", &test_node1 };
    PityUnit<PityModel> node3_test1 = PityUnit<PityModel>{ model.getNodeUnit(2), "test3", &test_node1 };

    model.rootUnit().run();
    //    pEpLog::log(model.rootUnit().to_string());
}