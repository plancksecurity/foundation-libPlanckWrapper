#include "PityModel.hh"
//#include "PityUnit.hh"
#include "iostream"

namespace pEp {
    namespace PityTest11 {
        bool PityModel::debug_log_enabled = true;

        PityModel::PityModel(const std::string& name, int nodesCount) :
            _name{ name }, _nodes_count{ nodesCount }, _root_unit{ nullptr, name, nullptr, this }
        {

            for (int i = 0; i < nodesCount; i++) {
                _nodes.emplace_back(PityNode(*this, i));
//                                _nodes.emplace_back(*this, i);
            }
        }

        std::string PityModel::getName() const
        {
            return _name;
        }

        std::vector<PityNode> PityModel::getNodes() const
        {
            return _nodes;
        }

        PityUnit<PityModel>& PityModel::rootUnit()
        {
            return _root_unit;
        }

    } // namespace PityTest11
} // namespace pEp
