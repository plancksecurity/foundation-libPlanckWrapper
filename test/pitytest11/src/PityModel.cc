#include "PityModel.hh"
#include "PityNode.hh"
#include <random>
#include <memory>

namespace pEp {
    namespace PityTest11 {
        bool PityModel::debug_log_enabled = false;

        PityModel::PityModel(const std::string& name, int nodeCount) : _name{ name }
        {
            for (int i = 0; i < nodeCount; i++) {
                _nodes.emplace_back(std::make_shared<PityNode>(i));
            }
        }

        std::string PityModel::getName() const
        {
            return _name;
        }

        std::vector<std::shared_ptr<PityNode>> PityModel::nodes() const
        {
            return _nodes;
        }

        PityNode* PityModel::nodeNr(int nr) const
        {
            return nodes().at(nr).get();
        }
    } // namespace PityTest11
} // namespace pEp
