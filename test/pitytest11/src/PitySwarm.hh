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

// PitySwarm creates a swarm of independent process nodes.
// Each node has its own perspective
// The perspective is a derivate of the model
// The model is the objective reality
// The perspective is the subjective reality

namespace pEp {
    namespace PityTest11 {
        class PitySwarm {
        public:
            using TestUnit = PityUnit<PityPerspective>;
            // Constructors
            explicit PitySwarm(const std::string& name, PityModel& model);

            TestUnit& addTestUnit(int nodeNr, const TestUnit& unit);
            void run();

            TestUnit& getSwarmUnit();

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityNode", debug_log_enabled };

        private:
            // methods
            void _createPerspective(const PityModel& model, PityPerspective* psp, int node_nr);
            int _init_process(TestUnit& unit, PityPerspective* ctx);

            // fields
            PityModel& _model;
            TestUnit _swarmUnit;
            // each node has
            std::vector<std::shared_ptr<PityPerspective>> _perspectives;
            std::map<int, TestUnit*> _leafunit;
            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif // PITYTEST_PITYSWARM_HH
