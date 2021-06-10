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
    PityUnit<> root = PityUnit<>{ nullptr, "test_processdirs" };

    // 1
    PityUnit<> test1 = PityUnit<>{
        &root,
        "node 1",
        [](PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/");
        }
    };

    PityUnit<> test1_1 = PityUnit<>{
        &test1,
        "node 1.1",
        []( PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/");
        }
    };

    // 2
    PityUnit<> test2 = PityUnit<>{
        &root,
        "node 2",
        []( PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/");
        }
    };

    PityUnit<> test2_1 = PityUnit<>{
        &test2,
        "node 2.1",
        []( PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_2_1/");
        },
        nullptr,
        nullptr,
        pEp::PityTest11::PityUnit<>::ExecutionMode::PROCESS_PARALLEL
    };


    PityUnit<> test2_1_1 = PityUnit<>{
        &test2_1,
        "node 2.1.1",
        [](PityUnit<> mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_2_1/");
        }
    };


    // 3
    PityUnit<> test3 = PityUnit<>{
        &root,
        "node 3",
        []( PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_3/");
        },
        nullptr,
        nullptr,
        PityUnit<>::ExecutionMode::PROCESS_PARALLEL
    };

    PityUnit<> test3_1 = PityUnit<>{
        &test3,
        "node 3.1",
        []( PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_3/");
        }
    };

    PityUnit<> test3_1_1 = PityUnit<>{
        &test3_1,
        "node 3.1.1",
        []( PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_3/");
        }
    };

    PityUnit<> test3_1_1_1 = PityUnit<>{
        &test3_1_1,
        "node 3.1.1",
        []( PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_3_1_1/");
        },
        nullptr,
        nullptr,
        PityUnit<>::ExecutionMode::PROCESS_PARALLEL
    };

    PityUnit<> test3_1_1_1_1 = PityUnit<>{
        &test3_1_1_1,
        "node 3.1.1.1",
        []( PityUnit<>& mynode, PityModel* model, PityPerspective* psp) {
            PTASSERT(mynode.processDir() == "./pitytest_data/test_processdirs/node_3_1_1/");
        }
    };

    root.run();
}