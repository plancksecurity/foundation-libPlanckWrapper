#include "../src/PityUnit.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace pEp::PityTest11;

void printHomeDir(const PityUnit<>& myself)
{
    //    TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid())));
    //    cout << "[" << to_string(getpid()) << "/" << myself.getFQName() << "] -  " << endl;
    setenv("HOME", myself.processDir().c_str(), 1);
    myself.log("HOME=" + string(getenv("HOME")));
}

void ptAssert(bool condition) {
    if(!condition) {

    }
}

int main(int argc, char* argv[])
{
    PityUnit<>::debug_log_enabled = false;
    PityUnit<>::setGlobalRootDir("./");
    PityUnit<> root = PityUnit<>{ nullptr, "rootnode" };

    PityUnit<> test1 = PityUnit<>{ &root, "node 1", [](const PityUnit<>& mynode) {
                                      assert(mynode.processDir() == "./rootnode/");
                                  } };

    PityUnit<> test1_1 = PityUnit<>{ &test1, "node 1.1", [](const PityUnit<>& mynode) {
                                        assert(mynode.processDir() == "./rootnode/");
                                    } };

    PityUnit<> test2 = PityUnit<>{ &root, "node 2", [](const PityUnit<>& mynode) {
                                      assert(mynode.processDir() == "./rootnode/");
                                  } };

    PityUnit<> test2_1 = PityUnit<>{ &test2,
                                     "node 2.1",
                                     [](const PityUnit<>& mynode) {
                                         assert(mynode.processDir() == "./rootnode/");
                                     },
                                     nullptr,
                                     pEp::PityTest11::PityUnit<>::ExecutionMode::PROCESS_PARALLEL };


    PityUnit<> test2_1_1 = PityUnit<>{ &test2_1, "node 2.1.1", [](PityUnit<> mynode) {
                                          assert(mynode.processDir() == "./rootnode/");
                                      } };

    root.run();
}