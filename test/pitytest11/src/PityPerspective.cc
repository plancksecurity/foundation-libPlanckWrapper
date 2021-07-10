#include "PityPerspective.hh"
#include "PityModel.hh"

namespace pEp {
    namespace PityTest11 {
        bool PityPerspective::debug_log_enabled = false;

        PityPerspective::PityPerspective(const PityModel& model) : model{ model }
        {
            pEpLogClass("called");
        }
    } // namespace PityTest11
} // namespace pEp
