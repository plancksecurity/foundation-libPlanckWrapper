// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYMODEL_HH
#define PITYTEST_PITYMODEL_HH

#include "../../../src/pEpLog.hh"
#include "PityNode.hh"
#include "PityUnit.hh"
#include <vector>
#include <memory>

namespace pEp {
    namespace PityTest11 {
        class PityModel {
        public:
            PityModel() = delete;
            PityModel(const std::string& name, int nodeCount);
            std::string getName() const;
            void setName(std::string name)   ;
            std::vector<std::shared_ptr<PityNode>> getNodes() const;
            PityUnit<PityModel>& rootUnit();
            PityUnit<PityModel>* getNodeUnit(int nr) const;

            PityNode* own_node = nullptr;

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityModel", debug_log_enabled };
        private:
            PityUnit<PityModel> _root_unit;
            std::vector<std::shared_ptr<PityNode>> _nodes;
            std::string _name;

            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11

}; // namespace pEp

#endif // PITYTEST_PITYMODEL_HH
