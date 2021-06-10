// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYMODEL_HH
#define PITYTEST_PITYMODEL_HH

#include "../../../src/pEpLog.hh"
#include "PityNode.hh"
#include "PityUnit.hh"
#include <vector>
#include <memory>

// The Model currently is as follows:
// The Model creates the TestTree using PityUnits.
// When creating the model you specify how many nodes you want
// The Model has a PityUnit rootUnit.
// The Model then creates a list of PityNodes, each containing a PityUnit connected to the rootUnit
// The PityNode, when run will modify the model to represent the perspective of the respective node nr.
// The perspective currently is complete by specifying a node, since there is a 1-1 node/ident relationship currently
//
// ATTENTION - TODO:
// Currently there is a strict 1-1 relationship of nodes and identities.
// One Node has exactly one identity, and this identity is only on this node.
// This needs to be enhanced to be a n-n relationship
// The Transport only addresses nodes, not idents, therefore
// If you have one ident on n nodes, the transport needs to check the model for all nodes the
// ident is on and send the message to all these nodes.
// If you have a node that has n identities, the persepective needs to specify node AND ident.

namespace pEp {
    namespace PityTest11 {
        class PityModel {
        public:
            // Constructors
            PityModel() = delete;
            PityModel(const std::string& name, int nodeCount);

            // Getters
            std::string getName() const;
            std::vector<std::shared_ptr<PityNode>> nodes() const;
//            PityUnit<PityModel>& unit();
//            PityUnit<PityModel>* unitOfNodeNr(int nr) const;
            PityNode* nodeNr(int nr) const;

            // Setter
            void setName(std::string name);

            // Perspective
            PityNode* own_node = nullptr;

            //Transport
//            bool hasMsg() const;
//            void sendMsg(const std::string nodename, const std::string& msg) const;
//            std::string pollMsg() const;
//            std::string receiveMsg(int timeout_msec = 100) const;

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityModel", debug_log_enabled };

        private:
            std::vector<std::shared_ptr<PityNode>> _nodes;
            std::string _name;

            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11

}; // namespace pEp

#endif // PITYTEST_PITYMODEL_HH
