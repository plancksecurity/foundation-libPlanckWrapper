#include "pEpTestModel.hh"
#include "iostream"

using namespace std;
namespace pEp {
    namespace Test {
        bool pEpTestModel::log_enabled = true;

        pEpTestModel::pEpTestModel(const string& name) : name(name) {}

        const std::string& pEpTestModel::getName() const
        {
            return name;
        }
    } // namespace Test
} // namespace pEp
