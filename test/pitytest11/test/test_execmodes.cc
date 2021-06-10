#include "../src/PityUnit.hh"
#include "../src/PityModel.hh"
#include "../src/PityPerspective.hh"
#include <iostream>

using namespace std;
using namespace pEp;
using namespace pEp::PityTest11;

void do_some_work(PityUnit<>& myself, int sleepmilis, int rep_count)
{
    int i = 0;
    while (i < rep_count) {
        myself.log(myself.getName() + " - " + to_string(i));
        Utils::sleep_millis(sleepmilis);
        i++;
    }
}

int main(int argc, char* argv[])
{
    {
        // DEBUG Logging of PityTestUnit itself
        PityUnit<>::debug_log_enabled = false;

        // The RootNode is the
        PityUnit<> root = PityUnit<>{ nullptr, "Test Execution Model" };

        // Subprocess 1
        PityUnit<> test1 = PityUnit<>{ &root,
                                       "node1",
                                       [](PityUnit<>& unit,
                                          PityModel* model,
                                          PityPerspective* psp) { do_some_work(unit, 200, 10); },
                                       nullptr,
                                       nullptr,
                                       pEp::PityTest11::PityUnit<>::ExecutionMode::PROCESS_PARALLEL };

        PityUnit<> test1_1 = PityUnit<>{ &test1,
                                         "test1.1",
                                         [](PityUnit<>& unit,
                                            PityModel* model,
                                            PityPerspective* psp) { do_some_work(unit, 200, 10); } };

        PityUnit<> test1_2 = PityUnit<>{ &test1,
                                         "test1.2",
                                         [](PityUnit<>& unit,
                                            PityModel* model,
                                            PityPerspective* psp) { do_some_work(unit, 200, 10); } };

        // Subprocess 2
        PityUnit<> test2 = PityUnit<>{ &root,
                                       "node2",
                                       [](PityUnit<>& unit,
                                          PityModel* model,
                                          PityPerspective* psp) { do_some_work(unit, 200, 10); },
                                       nullptr,
                                       nullptr,
                                       pEp::PityTest11::PityUnit<>::ExecutionMode::PROCESS_PARALLEL };

        PityUnit<> test2_1 = PityUnit<>{ &test2,
                                         "test2.1",
                                         [](PityUnit<>& unit,
                                            PityModel* model,
                                            PityPerspective* psp) { do_some_work(unit, 200, 10); } };

        PityUnit<> test2_2 = PityUnit<>{ &test2,
                                         "test2.2",
                                         [](PityUnit<>& unit,
                                            PityModel* model,
                                            PityPerspective* psp) { do_some_work(unit, 200, 10); } };

        root.run();
    }
}