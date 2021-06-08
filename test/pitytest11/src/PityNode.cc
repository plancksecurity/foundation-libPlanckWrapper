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
            pEpLogClass(std::string("called with: " + std::to_string(_node_nr) + "AT: " +ss.str()));

            _process_unit = std::make_shared<PityUnit<PityModel>>(
                &(model.rootUnit()),
                getName(),
                std::bind(&PityNode::_init,this, std::placeholders::_1),
                &model,
                PityUnit<PityModel>::ExecutionMode::PROCESS_PARALLEL);

        }

        void PityNode::_init(const PityUnit<PityModel>& unit)
        {
            unit.log("NODE INIT -  " + getName());
            unit.getModel()->own_node = this;
            unit.getModel()->setName("Copy for:" + getName());
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
            ret += "name: " +getName();
            return ret;
        }

        const std::shared_ptr<PityUnit<PityModel>>& PityNode::getProcessUnit() const
        {
            return _process_unit;
        }

        std::string PityNode::inboxDir() const {
            return getProcessUnit()->processDir() + "inbox/";
        }

    } // namespace PityTest11
} // namespace pEp
