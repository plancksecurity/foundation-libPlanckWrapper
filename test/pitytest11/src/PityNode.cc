#include "PityNode.hh"
#include <memory>
#include <sstream>

namespace pEp {
    namespace PityTest11 {
        bool PityNode::debug_log_enabled = false;

        PityNode::PityNode(int nodeNr) : _node_nr{ nodeNr }
        {
            logger_debug.set_instancename(getName());
            std::stringstream ss{};
            ss << this;
            pEpLogClass(std::string("called with: " + std::to_string(_node_nr) + "AT: " + ss.str()));
        }

        int PityNode::getNr() const
        {
            return _node_nr;
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
    } // namespace PityTest11
} // namespace pEp
