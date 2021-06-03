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


void printHomeDir(const pEpTestTree<>& myself)
{
    //    TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid())));
    //    cout << "[" << to_string(getpid()) << "/" << myself.getFQName() << "] -  " << endl;
    setenv("HOME", myself.processDir().c_str(), 1);
    myself.log("HOME=" + string(getenv("HOME")));
}


int main(int argc, char* argv[])
{
    pEpTestTree<>::debug_log_enabled = true;
    pEpTestTree<>::setGlobalRootDir("./testdata");
    pEpTestTree<> root = pEpTestTree<>{ nullptr, "rootnode"};

    pEpTestTree<> test1 = pEpTestTree<>{ &root, "node 1", [](const pEpTestTree<>& mynode) {
                                            printHomeDir(mynode);
                                        } };

    pEpTestTree<> test1_1 = pEpTestTree<>{ &test1, "node 1.1", [](const pEpTestTree<>& mynode) {
                                              printHomeDir(mynode);
                                          } };

    pEpTestTree<> test2 = pEpTestTree<>{ &root, "node 2", [](const pEpTestTree<>& mynode) {
                                            printHomeDir(mynode);
                                        } };

    pEpTestTree<> test2_1 = pEpTestTree<>{ &test2,
                                           "node 2.1",
                                           [](const pEpTestTree<>& mynode) { printHomeDir(mynode); },
                                           nullptr,
                                           pEp::Test::pEpTestTree<>::ExecutionMode::PROCESS_PARALLEL };


    pEpTestTree<> test2_1_1 = pEpTestTree<>{ &test2_1, "node 2.1.1", [](pEpTestTree<> mynode) {
                                                printHomeDir(mynode);
                                            } };

    root.run();
}