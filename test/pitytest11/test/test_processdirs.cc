#include "../src/PityUnit.hh"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace pEp::PityTest11;

void printHomeDir(PityUnit<>& myself)
{
    //    TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid())));
    //    cout << "[" << to_string(getpid()) << "/" << myself.getFQName() << "] -  " << endl;
    setenv("HOME", myself.processDir().c_str(), 1);
    myself.log("HOME=" + string(getenv("HOME")));
}

int main(int argc, char* argv[])
{
    PityUnit<>::debug_log_enabled = false;
    PityUnit<> root = PityUnit<>{ "test_processdirs" };

    // 1
    PityUnit<> test1 = PityUnit<>{ root, "node 1", [](PityUnit<>& mynode, void* ctx) {
                                      PTASSERT(
                                          mynode.processDir() == "./pitytest_data/test_processdirs/",
                                          "");
                                      return 0;
                                  } };

    PityUnit<> test1_1 = PityUnit<>{ test1, "node 1.1", [](PityUnit<>& mynode, void* ctx) {
                                        PTASSERT(
                                            mynode.processDir() == "./pitytest_data/test_processdirs/",
                                            "");
                                        return 0;
                                    } };

    // 2
    PityUnit<> test2 = PityUnit<>{ root, "node 2", [](PityUnit<>& mynode, void* ctx) {
                                      PTASSERT(
                                          mynode.processDir() == "./pitytest_data/test_processdirs/",
                                          "");
                                      return 0;
                                  } };
    PityUnit<> test2_1 = PityUnit<>{
        test2,
        "node 2.1",
        [](PityUnit<>& mynode, void* ctx) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_2_1/", "");
            return 0;
        },
        nullptr,
        pEp::PityTest11::PityUnit<>::ExecutionMode::PROCESS_PARALLEL
    };


    PityUnit<> test2_1_1 = PityUnit<>{ test2_1, "node 2.1.1", [](PityUnit<> mynode, void* ctx) {
                                          PTASSERT(
                                              mynode.processDir() ==
                                                  "./pitytest_data/test_processdirs/node_2_1/",
                                              "");
                                          return 0;
                                      } };


    // 3
    PityUnit<> test3 = PityUnit<>{
        root,
        "node 3",
        [](PityUnit<>& mynode, void* ctx) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_3/", "");
            return 0;
        },
        nullptr,
        PityUnit<>::ExecutionMode::PROCESS_PARALLEL
    };

    PityUnit<> test3_1 = PityUnit<>{ test3, "node 3.1", [](PityUnit<>& mynode, void* ctx) {
                                        PTASSERT(
                                            mynode.processDir() ==
                                                "./pitytest_data/test_processdirs/node_3/",
                                            "");
                                        return 0;
                                    } };

    PityUnit<> test3_1_1 = PityUnit<>{ test3_1, "node 3.1.1", [](PityUnit<>& mynode, void* ctx) {
                                          PTASSERT(
                                              mynode.processDir() ==
                                                  "./pitytest_data/test_processdirs/node_3/",
                                              "");
                                          return 0;
                                      } };

    PityUnit<> test3_1_1_1 = PityUnit<>{
        test3_1_1,
        "node 3.1.1",
        [](PityUnit<>& mynode, void* ctx) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_3_1_1/", "");
            return 0;
        },
        nullptr,
        PityUnit<>::ExecutionMode::PROCESS_PARALLEL
    };

    PityUnit<> test3_1_1_1_1 = PityUnit<>{
        test3_1_1_1,
        "node 3.1.1.1",
        [](PityUnit<>& mynode, void* ctx) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_3_1_1/", "");
            return 0;
        }
    };

    root.run();
}