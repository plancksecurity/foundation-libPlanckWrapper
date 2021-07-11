#include "PityNode.hh"
#include <memory>
#include <sstream>

namespace pEp {
    namespace PityTest11 {
        TestIdent::TestIdent(const std::string& addr) :
            addr{ addr }, did_rx_encrypted{ false }, did_tx_encrypted{ false }
        {
            ident = Test::Utils::createCptIdent(addr);
        }

        TestIdent::TestIdent(const TestIdent& rhs)
        {
           did_rx_encrypted = rhs.did_rx_encrypted;
           did_tx_encrypted = rhs.did_tx_encrypted;
           addr = rhs.addr;
           ident = Test::Utils::dup(rhs.ident.get());
        }

        bool TestIdent::tofu_done() const
        {
            return did_tx_encrypted && did_rx_encrypted;
        }


        // ---------------------------------------------------------------------------------

        bool PityNode::debug_log_enabled = false;

        PityNode::PityNode(int nodeNr) : _node_nr{ nodeNr }
        {
            logger_debug.set_instancename(getName());
            std::stringstream ss;
            ss << this;
            pEpLogClass(std::string("called with: " + std::to_string(_node_nr) + "AT: " + ss.str()));
            ident = std::make_shared<TestIdent>(TestIdent(getName()));
        }

        TestIdent& PityNode::getIdent() {
            return *ident.get();
        }

        int PityNode::getNr() const
        {
            return _node_nr;
        }

        std::string PityNode::getName() const
        {
            std::string ret;
            ret += "node_" + std::to_string(_node_nr) + "@peptest.org";
            return ret;
        }

        std::string PityNode::to_string() const
        {
            std::string ret;
            ret += "name: " + getName();
            return ret;
        }
    } // namespace PityTest11
} // namespace pEp
