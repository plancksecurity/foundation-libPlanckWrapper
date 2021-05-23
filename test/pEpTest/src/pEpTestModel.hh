// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PEPTESTMODEL_HH
#define LIBPEPADAPTER_PEPTESTMODEL_HH

#include "../../../src/pEpLog.hh"

namespace pEp {
    namespace Test {
        class pEpTestModel {
        public:
            pEpTestModel() = delete;
            pEpTestModel(const std::string& name);
            const std::string& getName() const;

        private:
            const std::string name;
            static bool log_enabled;
            Adapter::pEpLog::pEpLogger logger{ "pEpTestModel", log_enabled };
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger;
        };
    }; // namespace Test
};     // namespace pEp

#endif // LIBPEPADAPTER_PEPTESTMODEL_HH
