#include "../src/PityUnit.hh"
#include "../../framework/utils.hh"
#include "../../../src/std_utils.hh"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace pEp;
using namespace pEp::PityTest11;

void send(const PityUnit<>& myself)
{
    setenv("HOME", myself.processDir().c_str(), 1);
    myself.log("HOME=" + string(getenv("HOME")));
    ofstream msgfile = Utils::file_create(myself.processDir() + "/transport.msg");
    msgfile << "G4rbage" << endl;
    msgfile.close();
    sleep_millis(400000);
}

void receive(const PityUnit<>& myself)
{
    setenv("HOME", myself.processDir().c_str(), 1);
    myself.log("HOME=" + string(getenv("HOME")));
//    Utils::dir_list_files()
    sleep_millis(400000);
}

int main(int argc, char* argv[])
{
    PityUnit<>::debug_log_enabled = false;
    PityUnit<> root = PityUnit<>{ nullptr, "test_transport" };

    // 1
    PityUnit<> node1 = PityUnit<>{ &root,
                                   "node 1",
                                   [](const PityUnit<>& mynode) {

                                   },
                                   nullptr,
                                   pEp::PityTest11::PityUnit<>::ExecutionMode::PROCESS_PARALLEL };

    PityUnit<> node1_send = PityUnit<>{ &node1, "send", &send };


    // 2
    PityUnit<> node2 = PityUnit<>{ &root,
                                   "node 2",
                                   [](const PityUnit<>& mynode) {

                                   },
                                   nullptr,
                                   pEp::PityTest11::PityUnit<>::ExecutionMode::PROCESS_PARALLEL };

    PityUnit<> node2_receive = PityUnit<>{ &node2, "receive", &receive };

    //    root._init();
    root.run();
}