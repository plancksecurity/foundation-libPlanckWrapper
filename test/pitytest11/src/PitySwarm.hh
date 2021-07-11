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
// The perspective is a derivative of the model
// The model is the objective reality
// The perspective is the subjective reality

namespace pEp {
    namespace PityTest11 {
        class PitySwarm {
        public:
            using TestUnit = PityUnit<PityPerspective>;
            // Constructors
            explicit PitySwarm(const std::string& name, PityModel& model);

            PitySwarm(const PitySwarm& rhs, const std::string& new_name);

            TestUnit& addTestUnit(int nodeNr, const TestUnit& unit);

            TestUnit& getSwarmUnit();
            PitySwarm::TestUnit& getLeafUnit(int nodeNr);
            void run();



            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PitySwarm", debug_log_enabled };

        private:
            // methods
            PityPerspective _createPerspective(const PityModel& model, int node_nr);
            int _init_process(TestUnit& unit, PityPerspective* ctx);

            // fields
            PityModel& _model;
            TestUnit _swarmUnit;
            // each node has
            std::map<int, TestUnit*> _nodeUnits;
            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif // PITYTEST_PITYSWARM_HH
