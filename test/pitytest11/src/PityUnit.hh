// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYUNIT_HH
#define PITYTEST_PITYUNIT_HH

#include "../../../src/pEpLog.hh"
#include "../../../src/std_utils.hh"
#include "AbstractPityUnit.hh"
#include "fs_mutex.hh"
#include "PityTransport.hh"
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <functional>

namespace pEp {
    namespace PityTest11 {
        //TODO We need a context basetype
        class PityBaseCTX {
        };

        template<class TestContext = PityBaseCTX>
        class PityUnit : public AbstractPityUnit {
        public:
            // Test success if TestFunction:
            // * does not throw
            // * returns 0
            using TestFunction = std::function<int(PityUnit<TestContext>&, TestContext*)>;

            // Constructors
            PityUnit() = delete;
            explicit PityUnit<TestContext>(
                const std::string& name,
                TestFunction test_func = nullptr,
                TestContext* perspective = nullptr,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            explicit PityUnit<TestContext>(
                AbstractPityUnit& parent,
                const std::string& name,
                TestFunction test_func = nullptr,
                TestContext* perspective = nullptr,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            PityUnit<TestContext>(const PityUnit<TestContext> &rhs);

            // copy-assign
            PityUnit<TestContext>& operator=(const PityUnit<TestContext>& rhs);

            // clone
            PityUnit<TestContext> *clone() override;

            // Read-Only
            TestContext* getPerspective() const;

        private:
            void _runSelf() override;

            // Fields
            TestContext* _perspective; //nullptr if inherited
            TestFunction _test_func;
        };
    }; // namespace PityTest11
};     // namespace pEp

#include "PityUnit.hxx"

#endif // PITYTEST_PITYUNIT_HH
