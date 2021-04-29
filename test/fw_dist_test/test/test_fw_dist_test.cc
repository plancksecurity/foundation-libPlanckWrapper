#include "fw_dist_test.hh"
#include "../../framework/utils.hh"

#include <iostream>

using namespace std;
using namespace pEp;

Test::DistTest test_fw;


void alice_main()
{
    cout << "HYA FROM ALICE" << endl;
    while (true) {
        cout << "1" << endl;
        sleep_millis(1000);
    }
}

void bob_main()
{
    cout << "HYA FROM BOB" << endl;
    while (true) {
        cout << "2" << endl;
        sleep_millis(1000);
    }
}


int main(int argc, char* argv[])
{
    test_fw.mode = Test::DistTest::TestMode::ALICE_BOB;
    test_fw.alice_main = &::alice_main;
    test_fw.bob_main = &::bob_main;
    test_fw.run();


    cout << "HDUFGD" << endl;
}