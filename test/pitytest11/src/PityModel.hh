// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYMODEL_HH
#define PITYTEST_PITYMODEL_HH

#include "../../../src/pEpLog.hh"

namespace pEp {
    namespace PityTest11 {
        class PityModel {
        public:
            PityModel() = delete;
            PityModel(const std::string& name);
            virtual const std::string& getName() const;

        private:
            const std::string name;
            static bool log_enabled;
            Adapter::pEpLog::pEpLogger logger{ "PityModel", log_enabled };
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger;
        };
    }; // namespace Test
};     // namespace pEp

#endif // PITYTEST_PITYMODEL_HH
