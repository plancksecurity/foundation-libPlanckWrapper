#include "../src/pEpTestUnit.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>
//#include <regex>

using namespace std;
using namespace pEp;
using namespace pEp::Test;


void printHomeDir(pEpTestUnit& myself)
{
    //    TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid())));
    //    cout << "[" << to_string(getpid()) << "/" << myself.getFQName() << "] -  " << endl;
    setenv("HOME", myself.getDataDir().c_str(), 1);
    myself.log("HOME=" + string(getenv("HOME")));
}

int main(int argc, char* argv[])
{
    pEpTestModel model{ "TestTransport" };
    pEpTestUnit::log_enabled = false;
    pEpTestUnit::setDefaultDataRoot("./testdata/");
    {
        pEpTestUnit root = pEpTestUnit::createRootNode(
            model,
            "root node nr.1",
            [](pEpTestUnit mynode) { printHomeDir(mynode); },
            pEp::Test::pEpTestUnit::ExecutionMode::PROCESS_PARALLEL);

        pEpTestUnit test1 = pEpTestUnit::createChildNode(root, "node 1", [](pEpTestUnit mynode) {
            printHomeDir(mynode);
        });

        pEpTestUnit test1_1 = pEpTestUnit::createChildNode(test1, "node 1_1", [](pEpTestUnit mynode) {
            printHomeDir(mynode);
        });

        pEpTestUnit test2 = pEpTestUnit::createChildNode(root, "node 2", [](pEpTestUnit mynode) {
            printHomeDir(mynode);
        });

        pEpTestUnit test2_1 = pEpTestUnit::createChildNode(
            test2,
            "node 2_1",
            [](pEpTestUnit mynode) { printHomeDir(mynode); },
            pEp::Test::pEpTestUnit::ExecutionMode::PROCESS_PARALLEL);

        pEpTestUnit test2_1_1 = pEpTestUnit::createChildNode(test2_1, "node 1", [](pEpTestUnit mynode) {
            printHomeDir(mynode);
        });

        root.run();
    }
}