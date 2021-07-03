#include "../src/PityUnit.hh"
#include "../src/PityModel.hh"
#include "../src/PityPerspective.hh"
#include <iostream>

using namespace std;
using namespace pEp;
using namespace pEp::PityTest11;


struct CTXExecmodes {
    int sleepmilis;
    int rep_count;
};

using TestContext = CTXExecmodes;
using TestUnit = PityUnit<TestContext>;

int do_some_work(TestUnit& pity, TestContext* ctx)
{
    int i = 0;
    while (i < ctx->rep_count) {
        pity.log(pity.getName() + " - " + to_string(i));
        Utils::sleep_millis(ctx->sleepmilis);
        i++;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    // DEBUG Logging of PityTestUnit itself
    TestUnit::debug_log_enabled = false;
    CTXExecmodes ctxe;
    ctxe.sleepmilis = 100;
    ctxe.rep_count = 3;

    // The RootNode is the
    TestUnit root = TestUnit{ nullptr, "Test Execution Model" };

    // Subprocess 1
    TestUnit test1 = TestUnit{ &root,
                               "node1",
                               do_some_work,
                               &ctxe,
                               TestUnit::ExecutionMode::PROCESS_PARALLEL };

    TestUnit test1_1 = TestUnit{ &test1, "test1.1", do_some_work };

    TestUnit test1_2 = TestUnit{ &test1, "test1.2", do_some_work };

    // Subprocess 2
    TestUnit test2 = TestUnit{ &root,
                               "node2",
                               do_some_work,
                               &ctxe,
                               TestUnit::ExecutionMode::PROCESS_PARALLEL };

    TestUnit test2_1 = TestUnit{ &test2, "test2.1", do_some_work };

    TestUnit test2_2 = TestUnit{ &test2, "test2.2", do_some_work };

    root.run();
}