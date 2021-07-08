// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYUNIT_HXX
#define PITYTEST_PITYUNIT_HXX

#include "../../../src/std_utils.hh"
#include "PityUnit.hh"

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <functional>
#include <algorithm>
#include <sstream>
#include <exception>
#include <memory>
#include <unordered_map>
#include <sys/wait.h>


namespace pEp {
    namespace PityTest11 {

        template<class TestContext>
        PityUnit<TestContext>::PityUnit(
            const std::string &name,
            TestFunction test_func,
            TestContext *ctx,
            ExecutionMode exec_mode) :
            AbstractPityUnit(name, exec_mode),
            _ctx{ ctx }, _test_func{ test_func }
        {
        }

        template<class TestContext>
        PityUnit<TestContext>::PityUnit(
            AbstractPityUnit &parent,
            const std::string &name,
            TestFunction test_func,
            TestContext *ctx,
            ExecutionMode exec_mode) :
            AbstractPityUnit(parent, name, exec_mode),
            _ctx{ ctx }, _test_func{ test_func }
        {
        }

        template<class TestContext>
        PityUnit<TestContext>::PityUnit(const PityUnit<TestContext> &rhs) :
            AbstractPityUnit(rhs, *this)
        {
            _copyContext(rhs);
            _test_func = TestFunction(rhs._test_func);
        }

        template<class TestContext>
        PityUnit<TestContext> &PityUnit<TestContext>::operator=(const PityUnit<TestContext> &rhs)
        {
            _copyContext(rhs);
            _test_func = TestFunction(rhs._test_func);
            return *this;
        }

        template<class TestContext>
        PityUnit<TestContext> *PityUnit<TestContext>::clone()
        {
            return new PityUnit<TestContext>(*this);
        }

        template<class TestContext>
        void PityUnit<TestContext>::_runSelf()
        {
            if (_test_func != nullptr) {
                try {
                    _test_func(*this, getContext());
                    logH3(_status_string("\033[1m\033[32mSUCCESS" + Utils::to_termcol(_color())));
                } catch (const std::exception &e) {
                    _logRaw("reason: " + std::string(e.what()));
                    logH3(_status_string("\033[1m\033[31mFAILED" + Utils::to_termcol(_color())));
                }
            } else {
                _logRaw("No function to execute");
            }
        }

        // Inherited (if null see parent recursively)
        template<class TestContext>
        TestContext *PityUnit<TestContext>::getContext() const
        {
            pEpLogClass("called");
            TestContext *ret = nullptr;

            if (_ctx != nullptr) {
                ret = _ctx;
            } else {
                if (!isRoot()) {
                    ret = (dynamic_cast<PityUnit<TestContext> *>(getParent()))->getContext();
                }
            }
            return ret;
        }

        template<class TestContext>
        void PityUnit<TestContext>::_copyContext(const PityUnit<TestContext> &rhs)
        {
            auto *tmp = rhs.getContext();
            if (tmp != nullptr) {
                _owned_ctx = std::shared_ptr<TestContext>(new TestContext(*tmp));
                _ctx = tmp;
            } else {
                _ctx = nullptr;
            }
        }
    } // namespace PityTest11
} // namespace pEp


#endif // PITYTEST_PITYUNIT_HXX