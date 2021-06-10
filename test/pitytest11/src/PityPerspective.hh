// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYPERSPECTIVE_HH
#define PITYTEST_PITYPERSPECTIVE_HH

#include "../../../src/pEpLog.hh"

namespace pEp {
    namespace PityTest11 {
        class PityPerspective {
        public:
            // Constructors
            PityPerspective();

            // Perspective
            std::string name;
            std::string partner;
            std::vector<std::string> peers;

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityNode", debug_log_enabled };

        private:
            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif // PITYTEST_PITYPERSPECTIVE_HH
