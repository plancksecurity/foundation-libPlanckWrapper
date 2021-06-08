#include "PityModel.hh"
#include "PityNode.hh"
#include "PityUnit.hh"
#include "iostream"
#include <memory>
#include <functional>
#include <sstream>


namespace pEp {
    namespace PityTest11 {
        bool PityNode::debug_log_enabled = false;

        PityNode::PityNode(PityModel& model, int nodeNr) : _node_nr{ nodeNr }
        {
            logger_debug.set_instancename(getName());
            std::stringstream ss{};
            ss << this;
            pEpLogClass(std::string("called with: " + std::to_string(_node_nr) + "AT: " + ss.str()));

            _unit = std::make_shared<PityUnit<PityModel>>(
                &(model.unit()),
                getName(),
                std::bind(&PityNode::_init, this, std::placeholders::_1),
                &model,
                PityUnit<PityModel>::ExecutionMode::PROCESS_PARALLEL);
        }

        // We NEED to customize (perspective) the model here
        // This will be executed in the new process
        void PityNode::_init(const PityUnit<PityModel>& unit)
        {
            unit.log("NODE INIT -  " + getName());
            unit.getModel()->own_node = this;
            unit.getModel()->setName("Copy for:" + getName());

            _partnerAlgo_NextCircle();

            // Create peers, everyone but me
            auto nodes = _unit->getModel()->nodes();
            for (int i = 0; i < nodes.size(); i++) {
                if (i != _node_nr) {
                    peers.push_back(nodes.at(i)->getName());
                }
            }
            unit.log("NODE INIT DONE");
        }

        std::string PityNode::getName() const
        {
            std::string ret{};
            ret += "node_" + std::to_string(_node_nr) + "@peptest.org";
            return ret;
        }

        std::string PityNode::to_string() const
        {
            std::string ret{};
            ret += "name: " + getName();
            return ret;
        }

        const std::shared_ptr<PityUnit<PityModel>>& PityNode::unit() const
        {
            return _unit;
        }

        std::string PityNode::inboxDir() const
        {
            return unit()->processDir() + "inbox/";
        }

        void PityNode::_partnerAlgo_NextCircle() {
            // Default partner is next node, its a circle
            int partner_node_index = (_node_nr+1) % _unit->getModel()->nodes().size();
            partner = unit()->getModel()->nodes().at(partner_node_index)->getName();
        }
    } // namespace PityTest11
} // namespace pEp
