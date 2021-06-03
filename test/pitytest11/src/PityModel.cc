#include "PityModel.hh"
#include "iostream"

using namespace std;
namespace pEp {
    namespace PityTest11 {
        bool PityModel::log_enabled = true;

        PityModel::PityModel(const string& name) : name(name) {}

        const std::string& PityModel::getName() const
        {
            return name;
        }
    } // namespace Test
} // namespace pEp
