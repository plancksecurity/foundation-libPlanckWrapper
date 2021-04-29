#include "fw_dist_test.hh"
#include "../../../src/std_utils.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std;
namespace pEp {
    namespace Test {
        string dummy;

        DistTest::DistTest() : mode(TestMode::ALICE)
        {
            alice_home = data_dir + "/" + "alice/";
            bob_home = data_dir + "/" + "bob/";
            data_dir_recreate();
        }

        void DistTest::run()
        {
            if (mode == TestMode::ALICE) {
                alice_main();
            } else if (mode == TestMode::ALICE_BOB) {
                pid_t pid;
                pid = fork();

                if (pid == pid_t(0)) {
                    cout << "Child PID:" << pid << endl;
                    run_alice_main();
                } else if (pid > pid_t(0)) {
                    cout << "Parent PID:" << pid << endl;
                    run_bob_main();
                } else {
                    cerr << "error forking" << endl;
                }
            }
        }

        void DistTest::run_alice_main()
        {
            cout << "New process for Alice" << endl;
            setenv("HOME", alice_home.c_str(), 1);
            mkdir(alice_home.c_str(), 0770);
            cout << "HOME: " << getenv("HOME") << endl;
            alice_main();
        }

        void DistTest::run_bob_main()
        {
            cout << "New process for bob" << endl;
            setenv("HOME", bob_home.c_str(), 1);
            mkdir(bob_home.c_str(), 0770);
            cout << "HOME: " << getenv("HOME") << endl;
            bob_main();
        }


        void DistTest::data_dir_delete()
        {
            try {
                Utils::path_delete_all(data_dir);
            } catch (const exception& e) {
                TESTLOG("DistTest: - could not delete data dir: " + data_dir);
            }
        }

        void DistTest::data_dir_create()
        {
            Utils::dir_create(data_dir);
        }

        void DistTest::data_dir_recreate() {
            data_dir_delete();
            data_dir_create();
        };

    } // namespace Test
} // namespace pEp
