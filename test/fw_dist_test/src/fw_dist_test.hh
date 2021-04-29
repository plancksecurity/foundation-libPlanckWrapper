// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_FW_DIST_TEST_HH
#define LIBPEPADAPTER_FW_DIST_TEST_HH

#include <string>
#include <functional>

namespace pEp {
    namespace Test {

        class DistTest {
        public:
            DistTest();

            enum class TestMode
            {
                ALICE,
                ALICE_BOB
            };
            TestMode mode;

            std::string data_dir = "./data";
            std::string alice_home;
            std::string bob_home;

            std::function<void(void)> alice_main{};
            std::function<void(void)> bob_main{};
            void run();

        private:
            void run_alice_main();
            void run_bob_main();

            void data_dir_delete();
            void data_dir_create();
            void data_dir_recreate();
        };
    }; // namespace Test
};     // namespace pEp

#endif // LIBPEPADAPTER_FW_DIST_TEST_HH
