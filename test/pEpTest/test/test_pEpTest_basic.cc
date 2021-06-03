#include "../src/pEpTestTree.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace pEp;
using namespace pEp::Test;


void printHomeDir(const pEpTestTree<>& myself, int sleepmilis, int rep_count)
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
    {
        // DEBUG Logging of pEpTestTree itself
        pEpTestTree<>::debug_log_enabled = false;

        // Configure DataRoot for all TestNodes
        pEpTestTree<>::setGlobalRootDir("./basic_data_root");

        // The RootNode is the
        pEpTestTree<> root = pEpTestTree<>{ nullptr, "Test Execution Model"};

        // Subprocess 1
        pEpTestTree<> test1 = pEpTestTree<>{
            &root,
            "node1",
            [](const pEpTestTree<>& mynode) { printHomeDir(mynode, 200, 10); },
            nullptr,
            pEp::Test::pEpTestTree<>::ExecutionMode::PROCESS_PARALLEL
        };

        pEpTestTree<> test1_1 = pEpTestTree<>{ &test1, "test1.1", [](const pEpTestTree<>& mynode) {
                                                  printHomeDir(mynode, 200, 10);
                                              } };

        pEpTestTree<> test1_2 = pEpTestTree<>{ &test1, "test1.2", [](const pEpTestTree<>& mynode) {
                                                  printHomeDir(mynode, 200, 10);
                                              } };

        // Subprocess 2
        pEpTestTree<> test2 = pEpTestTree<>{
            &root,
            "node2",
            [](const pEpTestTree<>& mynode) { printHomeDir(mynode, 200, 10); },
            nullptr,
            pEp::Test::pEpTestTree<>::ExecutionMode::PROCESS_PARALLEL
        };

        pEpTestTree<> test2_1 = pEpTestTree<>{ &test2, "test2.1", [](const pEpTestTree<>& mynode) {
                                                  printHomeDir(mynode, 200, 10);
                                              } };

        pEpTestTree<> test2_2 = pEpTestTree<>{ &test2, "test2.2", [](const pEpTestTree<>& mynode) {
                                                  printHomeDir(mynode, 200, 10);
                                              } };

        root.run();
    }
}