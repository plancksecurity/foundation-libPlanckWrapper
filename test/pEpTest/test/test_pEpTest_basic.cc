#include "../src/pEpTestTree.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace pEp;
using namespace pEp::Test;


void printHomeDir(pEpTestTree& myself, int sleepmilis, int rep_count)
{
    //    TESTLOGH1("HYA FROM:" + myself.getFQName());
    int i = 0;
    while (i < rep_count) {
        myself.log(myself.getNodeName() + " - " + to_string(i));
        //        TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid()) + " - " + to_string(i)));
        sleep_millis(sleepmilis);
        i++;
    }
}

int main(int argc, char* argv[])
{
    string dummy;
    pEpTestModel model{ "pEpTestModel" };
    {
        // DEBUG Logging of pEpTestTree itself
        pEpTestTree::debug_log_enabled = true;

        // Configure DataRoot for all TestNodes
        pEpTestTree::setDataRoot("./basic_data_root");

        // The RootNode is the
        pEpTestTree root = pEpTestTree::createRootNode(
            model,
            "Test Execution Model",
            [](pEpTestTree mynode) { printHomeDir(mynode, 200, 5); });

        // Subprocess 1
        pEpTestTree test1 = pEpTestTree::createChildNode(
            root,
            "node1",
            [](pEpTestTree mynode) { printHomeDir(mynode, 200, 10); },
            pEp::Test::pEpTestTree::ExecutionMode::PROCESS_PARALLEL);

        pEpTestTree test1_1 = pEpTestTree::createChildNode(test1, "test1.1", [](pEpTestTree mynode) {
            printHomeDir(mynode, 200, 10);
        });

        pEpTestTree test1_2 = pEpTestTree::createChildNode(test1, "test1.2", [](pEpTestTree mynode) {
            printHomeDir(mynode, 200, 10);
        });

        // Subprocess 2
        pEpTestTree test2 = pEpTestTree::createChildNode(
            root,
            "node2",
            [](pEpTestTree mynode) { printHomeDir(mynode, 200, 10); },
            pEp::Test::pEpTestTree::ExecutionMode::PROCESS_PARALLEL);

        pEpTestTree test2_1 = pEpTestTree::createChildNode(test2, "test2.1", [](pEpTestTree mynode) {
          printHomeDir(mynode, 200, 10);
        });

        pEpTestTree test2_2 = pEpTestTree::createChildNode(test2, "test2.2", [](pEpTestTree mynode) {
          printHomeDir(mynode, 200, 10);
        });

        root.run();
    }
}