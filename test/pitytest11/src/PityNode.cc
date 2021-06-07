#include "PityModel.hh"
#include "PityNode.hh"
#include "PityUnit.hh"
#include "iostream"
#include <memory>

namespace pEp {
    namespace PityTest11 {
        bool PityNode::debug_log_enabled = false;

        PityNode::PityNode(PityModel& model, int nodeNr) :
            _model{ model }, _node_nr{ nodeNr }, _process_unit{
                std::make_shared<PityUnit<PityModel>>(
                    &(_model.rootUnit()),
                    getName(),
                    nullptr,
                    nullptr,
                    PityUnit<PityModel>::ExecutionMode::PROCESS_PARALLEL)
            }
        {
        }

        std::string PityNode::getName() const
        {
            std::string ret{};
            ret += "node_" + std::to_string(_node_nr);
            return ret;
        }

        std::string PityNode::to_string() const
        {
            std::string ret{};
            ret += "node_" + std::to_string(_node_nr);
            return ret;
        }

        const std::shared_ptr<PityUnit<PityModel>>& PityNode::getProcessUnit() const
        {
            return _process_unit;
        }
    } // namespace PityTest11
} // namespace pEp
