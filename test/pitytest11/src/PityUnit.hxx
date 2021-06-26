// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYUNIT_HXX
#define PITYTEST_PITYUNIT_HXX

#include "../../../src/std_utils.hh"
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
        // static
        template<class TestContext>
        std::string PityUnit<TestContext>::_global_root_dir = "./pitytest_data/";
        // static
        template<class TestContext>
        bool PityUnit<TestContext>::debug_log_enabled = false;
        // static
        template<class TestContext>
        int PityUnit<TestContext>::procUnitsCount = 0;

        // CONSTRUCTOR
        template<class TestContext>
        PityUnit<TestContext>::PityUnit(
            PityUnit<TestContext> *const parent,
            const std::string &name,
            TestFunction test_func,
            TestContext *perspective,
            ExecutionMode exec_mode) :
            _parent{ parent },
            _perspective{ perspective }, _name{ _normalizeName(name) }, _test_func{ test_func },
            _exec_mode{ exec_mode }
        {
            logger_debug.set_instancename(getPath());
            if (!_isRootUnit()) {
                parent->_addChildUnit(*this);
                // Inherit
                procUnitNr = _parent->procUnitNr;
                //Or update if procUnit
                if (_isProcessUnit()) {
                    _createTransport();
                    procUnitsCount++;
                    procUnitNr = procUnitsCount;
                }
            } else {
                procUnitNr = procUnitsCount;
            }
        }

        template<class TestContext>
        std::string PityUnit<TestContext>::getName() const
        {
            return _name;
        }

        template<class TestContext>
        std::string PityUnit<TestContext>::getPath() const
        {
            std::string ret;

            if (!_isRootUnit()) {
                ret = _parent->getPath() + "/" + getName();
            } else {
                ret = getName();
            }
            return ret;
        }

        // For:
        // RootUnit                         - "<name>"
        // ProcessUnit                      - ".../<proc>"
        // When Process as dir. parent      - ".../<proc>/name"
        // When no process as dir. parent   - ".../<proc>/.../name"
        template<class TestContext>
        std::string PityUnit<TestContext>::getPathShort() const
        {
            std::string ret;
            if (_isRootUnit()) {
                ret = getName();
            } else {
                if (_isProcessUnit()) {
                    ret += ".../" + getName();
                } else {
                    if (&(parentingProcessUnit()) == (_parent)) {
                        ret = parentingProcessUnit().getPathShort() + "/" + getName();
                    } else {
                        ret = parentingProcessUnit().getPathShort() + "/.../" + getName();
                    }
                }
            }
            return ret;
        }


        // Every RootUnit has its own dir
        template<class TestContext>
        std::string PityUnit<TestContext>::_rootUnitDir()
        {
            return getGlobalRootDir() + rootUnit()->getName() + "/";
        }

        // Every process has its own dir inside its rootUnitDir
        // All other units inherit processDir from their Root/ProcessUnit
        template<class TestContext>
        std::string PityUnit<TestContext>::processDir()
        {
            if (_isRootUnit()) {
                return _rootUnitDir();
            } else {
                if (_isProcessUnit()) {
                    return _rootUnitDir() + getName() + "/";
                } else {
                    return _parent->processDir();
                }
            }
        }

        // Every process has its own dir inside its rootUnitDir
        // All other units inherit transportDir from their Root/ProcessUnit
        template<class TestContext>
        std::string PityUnit<TestContext>::transportDir()
        {
            if (_isRootUnit()) {
                throw std::runtime_error("No transport dir");
            } else {
                if (_isProcessUnit()) {
                    return processDir() + "inbox/";
                } else {
                    return _parent->transportDir();
                }
            }
        }

        //        template<class TestContext>
        //        void PityUnit<TestContext>::setExecutionMode(ExecutionMode mode)
        //        {
        //            _exec_mode = mode;
        //        }

        // static
        template<class TestContext>
        void PityUnit<TestContext>::setGlobalRootDir(const std::string &dir)
        {
            PityUnit<TestContext>::_global_root_dir = dir;
        }

        // static
        template<class TestContext>
        std::string PityUnit<TestContext>::getGlobalRootDir()
        {
            return PityUnit::_global_root_dir;
        }

        template<class TestContext>
        void PityUnit<TestContext>::run()
        {
            pEpLogClass("called");
            _log_mutex = std::make_shared<fs_mutex>("fds");
            _log_mutex->release();

            if (_isRootUnit()) {
                _init();
            }

            // Execute in fork and wait here until process ends
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL) { // fork
                _executeInFork(std::bind(&PityUnit<TestContext>::_run, this), true);
                // Execute in fork and go on, wait for process execution in the end
            } else if (_exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                _executeInFork(std::bind(&PityUnit<TestContext>::_run, this), false);
                // Execute as normal funciton
            } else if (_exec_mode == ExecutionMode::FUNCTION) {
                _run();
            } else if (_exec_mode == ExecutionMode::THREAD_PARALLEL) {
                throw std::invalid_argument(to_string(_exec_mode) + " - not implemented");
            } else if (_exec_mode == ExecutionMode::THREAD_SEQUENTIAL) {
                throw std::invalid_argument(to_string(_exec_mode) + " - not implemented");
            }

            if (_isRootUnit()) {
                _waitChildProcesses();
            }
        }

        template<class TestContext>
        std::string PityUnit<TestContext>::to_string(bool recursive, int indent)
        {
            std::string ret;
            std::stringstream builder;
            builder << std::string(indent * 4, ' ');

            builder << getName();
            builder << " [ ";
            builder << to_string(_exec_mode) << " - ";
            builder << "\"" << processDir() << "\"";
            builder << " ]";
            builder << std::endl;
            ret = builder.str();

            if (recursive) {
                if (!_children.empty()) {
                    indent++;
                    for (const std::pair<std::string, PityUnit<TestContext> &> child : _children) {
                        ret += child.second.to_string(true, indent);
                    }
                    indent--;
                }
            }
            return ret;
        }

        template<class TestContext>
        std::string PityUnit<TestContext>::to_string(const ExecutionMode &emode)
        {
            switch (emode) {
                case ExecutionMode::FUNCTION:
                    return "FUNCTION";
                case ExecutionMode::PROCESS_SEQUENTIAL:
                    return "PROC_SEQ";
                case ExecutionMode::PROCESS_PARALLEL:
                    return "PROC_PAR";
                case ExecutionMode::THREAD_SEQUENTIAL:
                    return "THREAD_S";
                case ExecutionMode::THREAD_PARALLEL:
                    return "THREAD_P";
                case ExecutionMode::INHERIT:
                    return "INHERIT";
                default:
                    return "UNDEFINED EXECUTION MODE";
            }
        }

        template<class TestContext>
        void PityUnit<TestContext>::recreateDirsRecursively()
        {
            Utils::dir_recreate(processDir());
            if (!_children.empty()) {
                for (const std::pair<std::string, PityUnit<TestContext> &> child : _children) {
                    child.second.recreateDirsRecursively();
                }
            }
        }

        template<class TestContext>
        void PityUnit<TestContext>::registerAsTransportEndpoint()
        {
            transportEndpoints().insert({ getName(), transportDir() });
        }

        template<class TestContext>
        Endpoints &PityUnit<TestContext>::transportEndpoints()
        {
            if (_isRootUnit()) {
                return _transport_endpoints;
            } else {
                return rootUnit()->transportEndpoints();
            }
        }


        template<class TestContext>
        void PityUnit<TestContext>::log(const std::string &msg) const
        {
            std::stringstream builder;
            builder << "[ ";
            builder << std::to_string(getpid());
            builder << " - ";
            builder << getPathShort();
            builder << " ] -  ";
            builder << msg;

            _logRaw(builder.str());
        }


        template<class TestContext>
        void PityUnit<TestContext>::logH1(const std::string &msg) const
        {
            Adapter::pEpLog::logH1(msg, _termColor());
        }

        template<class TestContext>
        void PityUnit<TestContext>::logH2(const std::string &msg) const
        {
            Adapter::pEpLog::logH2(msg, _termColor());
        }

        template<class TestContext>
        void PityUnit<TestContext>::logH3(const std::string &msg) const
        {
            Adapter::pEpLog::logH3(msg, _termColor());
        }


        // PRIVATE ---------------------------------------------------------------------------------
        template<class TestContext>
        void PityUnit<TestContext>::_init()
        {
            logH1("PityTest Starting...");
            _logRaw("RootUnit: " + getPathShort());
            _logRaw("GlobalRootDir: " + getGlobalRootDir());
            _logRaw("\nTestTree");
            _logRaw("--------");
            _logRaw(to_string());

            logH3("INIT");
            Utils::dir_ensure(getGlobalRootDir());
            recreateDirsRecursively();
            logH3("INIT DONE");
        }


        template<class TestContext>
        void PityUnit<TestContext>::_run()
        {
            logH2(_status_string("STARTING"));
            _runSelf();
            _runChildren();
        }

        template<class TestContext>
        void PityUnit<TestContext>::_runSelf()
        {
            if (_test_func != nullptr) {
                try {
                    _test_func(*this, getPerspective());
                    logH3(_status_string("\033[1m\033[32mSUCCESS" + Utils::to_termcol(_termColor())));
                } catch (const std::exception &e) {
                    _logRaw("reason: " + std::string(e.what()));
                    logH3(_status_string("\033[1m\033[31mFAILED" + Utils::to_termcol(_termColor())));
                }
            } else {
                _logRaw("No function to execute");
            }
        }

        template<class TestContext>
        void PityUnit<TestContext>::_runChildren() const
        {
            if (!_children.empty()) {
                for (const std::pair<std::string, PityUnit<TestContext> &> child : _children) {
                    child.second.run();
                }
            }
        }

        template<class TestContext>
        void PityUnit<TestContext>::_executeInFork(std::function<void(void)> func, bool wait_child) const
        {
            pid_t pid;
            pid = fork();
            if (pid == pid_t(0)) {
                func();
                exit(0);
            } else if (pid < pid_t(0)) {
                throw std::runtime_error("Error forking");
            }
            if (wait_child) {
                _waitChildProcesses();
            }
        }

        template<class TestContext>
        void PityUnit<TestContext>::_waitChildProcesses() const
        {
            int status;
            pid_t pid;
            while ((pid = wait(&status)) > 0) {
                std::string color;
                if(status == 0) {
                    color ="\033[1m\033[32m"; // Green
                } else {
                    color ="\033[1m\033[31m"; // Red
                }
                logH3(
                    color + "PROCESS [ " + std::to_string((int)pid) +
                    " ] EXITED with status code: " + std::to_string(status) +
                    Utils::to_termcol(_termColor()));
            }
        }

        template<class TestContext>
        void PityUnit<TestContext>::_addChildUnit(PityUnit<TestContext> &unit)
        {
            _children.insert(std::pair<std::string, PityUnit<TestContext> &>(unit.getName(), unit));
        }

        template<class TestContext>
        bool PityUnit<TestContext>::_isProcessUnit() const
        {
            bool ret = false;
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL ||
                _exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                ret = true;
            }
            return ret;
        }

        template<class TestContext>
        bool PityUnit<TestContext>::_isRootUnit() const
        {
            if (_parent == nullptr) {
                return true;
            } else {
                return false;
            }
        }

        template<class TestContext>
        PityUnit<TestContext> *PityUnit<TestContext>::rootUnit()
        {
            //            const PityUnit<TestContext>* ret = nullptr;
            if (!_isRootUnit()) {
                return _parent->rootUnit();
            } else {
                return this;
            }
            //            assert(ret != nullptr);
            // cant be null because for createChildUnit() you need to provide a TestUnit& and
            // the only other way is using createRootUnit() which has parent == nullptr
            //            return *ret;
        }

        template<class TestContext>
        const PityUnit<TestContext> &PityUnit<TestContext>::parentingProcessUnit() const
        {
            if (_isRootUnit() || _isProcessUnit()) {
                return *this;
            } else {
                return _parent->parentingProcessUnit();
            }
        }


        // Inherited (if null see parent recursively)
        template<class TestContext>
        TestContext *PityUnit<TestContext>::getPerspective() const
        {
            pEpLogClass("called");
            TestContext *ret = nullptr;

            if (_perspective != nullptr) {
                ret = _perspective;
            } else {
                if (!_isRootUnit()) {
                    ret = _parent->getPerspective();
                }
            }
            return ret;
        }

        // Inherited (if null see parent recursively)
        template<class TestContext>
        void PityUnit<TestContext>::_createTransport()
        {
            registerAsTransportEndpoint();
            _transport = std::make_shared<PityTransport>(transportDir(), transportEndpoints());
        }

        // Inherited (if null see parent recursively)
        template<class TestContext>
        PityTransport *PityUnit<TestContext>::transport() const
        {
            pEpLogClass("called");
            PityTransport *ret = nullptr;

            if (_transport.get() != nullptr) {
                ret = _transport.get();
            } else {
                if (!_isRootUnit()) {
                    ret = _parent->transport();
                }
            }
            return ret;
        }

        // name is alphanumeric only (everything else will be replaced by an underscore)
        // static
        template<class TestContext>
        std::string PityUnit<TestContext>::_normalizeName(std::string name)
        {
            replace_if(
                name.begin(),
                name.end(),
                [](char c) -> bool { return !isalnum(c); },
                '_');

            return name;
        }

        template<class TestContext>
        std::string PityUnit<TestContext>::_status_string(const std::string &msg) const
        {
            std::string ret;
            ret = "[ " + to_string(_exec_mode) + ":" + std::to_string(getpid()) + " ]  [ " +
                  getPathShort() + " ]  [ " + msg + " ]";
            return ret;
        }


        template<class TestContext>
        Utils::Color PityUnit<TestContext>::_colForProcUnitNr(int procUnitNr) const
        {
            int nrColors = 7;
            switch (procUnitNr % nrColors) {
                case 0:
                    return Utils::Color::WHITE;
                case 1:
                    return Utils::Color::CYAN;
                case 2:
                    return Utils::Color::MAGENTA;
                case 3:
                    return Utils::Color::BLUE;
                case 4:
                    return Utils::Color::GREEN;
                case 5:
                    return Utils::Color::YELLOW;
                case 6:
                    return Utils::Color::RED;
                default:
                    return Utils::Color::WHITE;
            }
        }

        template<class TestContext>
        Utils::Color PityUnit<TestContext>::_termColor() const
        {
            return _colForProcUnitNr(procUnitNr);
        }

        template<class TestContext>
        void PityUnit<TestContext>::_logRaw(const std::string &msg) const
        {
            _log_mutex->aquire();
            Adapter::pEpLog::log(msg, _termColor());
            _log_mutex->release();
        }

    } // namespace PityTest11
} // namespace pEp


#endif // PITYTEST_PITYUNIT_HXX