// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYPERSPECTIVE_HH
#define PITYTEST_PITYPERSPECTIVE_HH

#include "../../../src/pEpLog.hh"
#include "../../framework/test_utils.hh"
#include "PityModel.hh"
#include <map>

namespace pEp {
    namespace PityTest11 {

        // Group
        struct Group {
            std::string addr;
            std::string moderator;
            std::vector<TestIdent> members;

        };

        class PityPerspective {
        public:
            // Constructors
            PityPerspective(const PityModel& model);

            // Lets grant access to the whole model too
            const PityModel& model;

            TestIdent* getPeer(const std::string& addr);
            // Perspective
            std::string own_name;
//            TestIdent* cpt = nullptr;
            void setPeerNrAsCpt(int nr);
            TestIdent& getCpt();
            std::vector<TestIdent> peers;

            Test::Utils::pEpIdent own_ident;
//            Test::Utils::pEpIdent cpt_ident;

            // Groups
            Group* getGroup(const std::string& addr);
            std::vector<Group> groups;

            //Callbacks
            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityNode", debug_log_enabled };

        private:
            int peerNrAsCpt;
            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif // PITYTEST_PITYPERSPECTIVE_HH
