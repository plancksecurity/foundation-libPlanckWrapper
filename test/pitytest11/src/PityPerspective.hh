// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYPERSPECTIVE_HH
#define PITYTEST_PITYPERSPECTIVE_HH

#include "../../../src/pEpLog.hh"
#include "../../framework/utils.hh"
#include "PityModel.hh"

namespace pEp {
    namespace PityTest11 {
        class PityPerspective {
        public:
            // Constructors
            PityPerspective(PityModel& model);

            // Lets grant access to the whole model too
            PityModel& model;

            // Perspective
            std::string own_name;
            std::string cpt_name;
            std::vector<std::string> peers;

            Test::Utils::pEpIdent own_ident;
            Test::Utils::pEpIdent cpt_ident;

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityNode", debug_log_enabled };

        private:
            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif // PITYTEST_PITYPERSPECTIVE_HH
