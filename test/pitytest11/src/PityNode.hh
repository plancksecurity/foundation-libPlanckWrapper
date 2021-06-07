// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYNODE_HH
#define PITYTEST_PITYNODE_HH

#include "../../../src/pEpLog.hh"
#include "PityUnit.hh"
#include "PityModel.hh"

namespace pEp {
    namespace PityTest11 {
        class PityModel;
        class PityNode {
        public:
            PityNode() = delete;
            explicit PityNode(PityModel& model, int nodeNr);
            std::string getName() const;
            std::string to_string() const;

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityNode", debug_log_enabled };
        private:
            const int _node_nr;
            PityModel& _model;
            std::shared_ptr<PityUnit<PityModel>> _process_unit;

        public:
            const std::shared_ptr<PityUnit<PityModel>>& getProcessUnit() const;

        private:
            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif // PITYTEST_PITYNODE_HH
