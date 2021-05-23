#include "../src/pEpTestUnit.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace pEp;
using namespace pEp::Test;


void print_and_sleep(pEpTestUnit& myself, int sleepmilis, int rep_count)
{
//    TESTLOGH1("HYA FROM:" + myself.getFQName());
    int i = 0;
    while (i < rep_count) {
        TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid()) + " - " + to_string(i)));
        sleep_millis(sleepmilis);
        i++;
    }
}

int main(int argc, char* argv[])
{
    string dummy;
    pEpTestModel model{ "pEpTestModel" };
    {
        //        pEpTestUnit::setDefaultExecutionMode(pEp::Test::pEpTestUnit::ExecutionMode::PROCESS_SERIAL);
        pEpTestUnit root = pEpTestUnit::createRootNode(
            model,
            "DistributedTest",
            [](pEpTestUnit mynode) { print_and_sleep(mynode, 200, 5); },
            pEp::Test::pEpTestUnit::ExecutionMode::PROCESS_PARALLEL);

        pEpTestUnit test1 = pEpTestUnit::createChildNode(root, "node1", [](pEpTestUnit mynode) {
            print_and_sleep(mynode, 200, 10);
        });

        pEpTestUnit test2 = pEpTestUnit::createChildNode(root, "node2", [](pEpTestUnit mynode) {
            print_and_sleep(mynode, 200, 10);
        });

        pEpTestUnit test1_1 = pEpTestUnit::createChildNode(test1, "test1.1", [](pEpTestUnit mynode) {
            print_and_sleep(mynode, 200, 10);
        });

        pEpTestUnit test2_1 = pEpTestUnit::createChildNode(test2, "test2.1", [](pEpTestUnit mynode) {
            print_and_sleep(mynode, 200, 10);
        });

        //        cout << root.to_string() << endl;
        root.run();
    }
}