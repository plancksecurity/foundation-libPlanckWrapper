// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYMODEL_HH
#define PITYTEST_PITYMODEL_HH

#include "../../../src/pEpLog.hh"
#include "PityNode.hh"
#include "PityUnit.hh"

namespace pEp {
    namespace PityTest11 {
        class PityModel {
        public:
            PityModel() = delete;
            PityModel(const std::string& name, int nodesCount);
            std::string getName() const;
            std::vector<PityNode> getNodes() const;
            PityUnit<PityModel>& rootUnit();


            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityModel", debug_log_enabled };
        private:
            const int _nodes_count;
            PityUnit<PityModel> _root_unit;
            std::vector<PityNode> _nodes;
            const std::string _name;

            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11

}; // namespace pEp

#endif // PITYTEST_PITYMODEL_HH
