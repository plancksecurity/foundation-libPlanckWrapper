#include "../src/pEpTestTree.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
//#include <regex>

using namespace std;
using namespace pEp;
using namespace pEp::Test;


void printHomeDir(pEpTestTree& myself)
{
    //    TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid())));
    //    cout << "[" << to_string(getpid()) << "/" << myself.getFQName() << "] -  " << endl;
    setenv("HOME", myself.getDataDir().c_str(), 1);
    myself.log("HOME=" + string(getenv("HOME")));
}

int main(int argc, char* argv[])
{
    pEpTestModel model{ "TestTransport" };
//    pEpTestTree::debug_log_enabled = true;
    pEpTestTree::setDataRoot("./testdata/");
    {
        pEpTestTree root = pEpTestTree::createRootNode(
            model,
            "root node nr.1",
            [](pEpTestTree mynode) { printHomeDir(mynode); },
            pEp::Test::pEpTestTree::ExecutionMode::PROCESS_PARALLEL);

        pEpTestTree test1 = pEpTestTree::createChildNode(root, "node 1", [](pEpTestTree mynode) {
            printHomeDir(mynode);
        });

        pEpTestTree test1_1 = pEpTestTree::createChildNode(test1, "node 1.1", [](pEpTestTree mynode) {
            printHomeDir(mynode);
        });

        pEpTestTree test2 = pEpTestTree::createChildNode(root, "node 2", [](pEpTestTree mynode) {
            printHomeDir(mynode);
        });

        pEpTestTree test2_1 = pEpTestTree::createChildNode(
            test2,
            "node 2.1",
            [](pEpTestTree mynode) { printHomeDir(mynode); },
            pEp::Test::pEpTestTree::ExecutionMode::PROCESS_PARALLEL);

        pEpTestTree test2_1_1 = pEpTestTree::createChildNode(test2_1, "node 2.1.1", [](pEpTestTree mynode) {
            printHomeDir(mynode);
        });

        root.run();
    }
}