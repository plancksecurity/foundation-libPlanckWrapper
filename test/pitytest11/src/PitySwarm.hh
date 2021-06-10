// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYSWARM_HH
#define PITYTEST_PITYSWARM_HH

#include "PityModel.hh"
#include "PityUnit.hh"
#include "PityPerspective.hh"
#include "../../../src/pEpLog.hh"
#include <vector>
#include <memory>
#include <functional>

namespace pEp {
    namespace PityTest11 {
        class PitySwarm {
        public:
            // Constructors
            PitySwarm(PityModel& model);

            void addTestUnit(
                int nodeNr,
                const std::string& name,
                PityUnit<PityPerspective>::TestFunction test_func);

            //Run
            void run();

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityNode", debug_log_enabled };

        private:
            PityModel& _model;
            std::shared_ptr<PityUnit<PityPerspective>> _rootUnit;
            std::vector<std::shared_ptr<PityUnit<PityPerspective>>> _nodeUnits;
            std::vector<std::shared_ptr<PityUnit<PityPerspective>>> _testUnits;
            std::vector<std::shared_ptr<PityPerspective>> _perspectives;

            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif // PITYTEST_PITYSWARM_HH
