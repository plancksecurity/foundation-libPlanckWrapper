// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYNODE_HH
#define PITYTEST_PITYNODE_HH

#include "../../../src/pEpLog.hh"
#include "../../framework/utils.hh"

namespace pEp {
    namespace PityTest11 {
        class TestIdent {
        public:
            TestIdent() = delete;
            explicit TestIdent(const std::string& addr);
            TestIdent(const TestIdent& rhs);
            bool tofu_done() const;

            std::string addr;
            pEp::Test::Utils::pEpIdent ident{};

            // state
            bool did_tx_encrypted;
            bool did_rx_encrypted;
        };


        class PityNode {
        public:
            // Constructors
            PityNode() = delete;
            explicit PityNode(int nodeNr);

            // Getters
            TestIdent& getIdent();
            int getNr() const;
            std::string getName() const;
            std::string to_string() const;

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityNode", debug_log_enabled };

        private:
            //fields
            const int _node_nr;
            std::shared_ptr<TestIdent> ident;

            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif // PITYTEST_PITYNODE_HH
