#include "../src/PityTest.hh"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace pEp::PityTest11;


using TestContext = void;
using TestUnit = PityUnit<>;

//TODO: Add HOME testing
void printHomeDir(TestUnit& myself)
{
    //    TESTLOG(string(myself.getFQName() + " - PID: " + to_string(getpid())));
    //    cout << "[" << to_string(getpid()) << "/" << myself.getFQName() << "] -  " << endl;
    setenv("HOME", myself.getProcessDir().c_str(), 1);
    myself.log("HOME=" + string(getenv("HOME")));
}

int main(int argc, char* argv[])
{
    PityUnit<>::debug_log_enabled = false;
    // Suite
    TestUnit suite = TestUnit{ "test_processdirs" };

    // 1
    suite
        .addNew<TestUnit>(
            "node 1",
            [](TestUnit& pity, TestContext* ctx) {
                PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/", "node 1");
                return 0;
            })
        .addNew<TestUnit>("node 1.1", [](TestUnit& pity, TestContext* ctx) {
            PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/", "node 1.1");
            return 0;
        });

    // 2
    suite
        .addNew<TestUnit>(
            "node 2",
            [](TestUnit& pity, TestContext* ctx) {
                PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/", "node 2");
                return 0;
            })
        .addNew<TestUnit>(
            "node 2.1",
            [](TestUnit& pity, TestContext* ctx) {
                PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/node_2_1/", "");
                return 0;
            },
            nullptr,
            PityUnit<>::ExecutionMode::PROCESS_PARALLEL)
        .addNew<TestUnit>("node 2.1.1", [](TestUnit& pity, TestContext* ctx) {
            PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/node_2_1/", "");
            return 0;
        });


    // 3
    suite
        .addNew<TestUnit>(
            "node 3",
            [](TestUnit& pity, TestContext* ctx) {
                PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/node_3/", "");
                return 0;
            },
            nullptr,
            PityUnit<>::ExecutionMode::PROCESS_PARALLEL)
        .addNew<TestUnit>(
            "node 3.1",
            [](TestUnit& pity, TestContext* ctx) {
                PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/node_3/", "");
                return 0;
            })
        .addNew<TestUnit>(
            "node 3.1.1",
            [](TestUnit& pity, TestContext* ctx) {
                PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/node_3/", "");
                return 0;
            })
        .addNew<TestUnit>(
            "node 3.1.1",
            [](TestUnit& pity, TestContext* ctx) {
                PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/node_3_1_1/", "");
                return 0;
            },
            nullptr,
            PityUnit<>::ExecutionMode::PROCESS_PARALLEL)
        .addNew<TestUnit>("node 3.1.1.1", [](TestUnit& pity, TestContext* ctx) {
            PITYASSERT(pity.getProcessDir() == "./pitytest_data/test_processdirs/node_3_1_1/", "");
            return 0;
        });

    suite.run();
}