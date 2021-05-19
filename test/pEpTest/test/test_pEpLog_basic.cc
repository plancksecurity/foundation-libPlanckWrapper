#include "pEpTestUnit.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace pEp;
using namespace pEp::Test;


void print_and_sleep(pEpTestUnit myself, int sleepmilis, int rep_count)
{
    TESTLOGH1("HYA FROM:" + myself.getFQName());
    int i = 0;
    while (i < rep_count) {
        TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid()) + " - " + to_string(i)));
        sleep_millis(sleepmilis);
        i++;
    }
}

int main(int argc, char* argv[])
{
    pEpTestModel model{};
    {
        pEpTestUnit::setDefaultExecutionMode(pEpTestUnit::ExecutionMode::PROCESS_PARALLEL);
        auto root = pEpTestUnit::createRootNode(model, "rootNode", [](pEpTestUnit mynode) {
            print_and_sleep(mynode, 100, 10);
        });

        auto test1 = pEpTestUnit::createChildNode(root, "test1", [](pEpTestUnit mynode) -> void {
            print_and_sleep(mynode, 50, 23);
        }, pEp::Test::pEpTestUnit::ExecutionMode::PROCESS_SERIAL);

        auto test2 = pEpTestUnit::createChildNode(root, "test2", [](pEpTestUnit mynode) -> void {
            print_and_sleep(mynode, 150, 13);
        });

        auto test1_1 = pEpTestUnit::createChildNode(test1, "test1.1", nullptr);

        root.run();
    }
}