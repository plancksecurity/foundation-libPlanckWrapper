#include "PityModel.hh"
//#include "PityUnit.hh"
#include "iostream"

namespace pEp {
    namespace PityTest11 {
        bool PityModel::debug_log_enabled = true;

        PityModel::PityModel(const std::string& name, int nodeCount) :
            _name{ name }, _root_unit{ nullptr, name, nullptr, this }
        {
            for (int i = 0; i < nodeCount; i++) {
                auto tmp = std::make_shared<PityNode>(*this, i);
                _nodes.emplace_back(tmp);
            }
        }

        std::string PityModel::getName() const
        {
            return _name;
        }

        void PityModel::setName(std::string name)
        {
            _name = name;
        }

        std::vector<std::shared_ptr<PityNode>> PityModel::getNodes() const
        {
            return _nodes;
        }

        PityUnit<PityModel>& PityModel::rootUnit()
        {
            return _root_unit;
        }

        PityUnit<PityModel>* PityModel::getNodeUnit(int nr) const
        {
            return getNodes().at(nr)->getProcessUnit().get();
        }

    } // namespace PityTest11
} // namespace pEp
