// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "AbstractPityUnit.hh"
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
        std::string AbstractPityUnit::_global_root_dir = "./pitytest_data/";
        // static
        bool AbstractPityUnit::debug_log_enabled = false;
        // static
        int AbstractPityUnit::procUnitsCount = 0;

        AbstractPityUnit::AbstractPityUnit(const std::string &name, ExecutionMode exec_mode) :
            PityTree<AbstractPityUnit>(*this, name),
            _exec_mode{ exec_mode }
        {
            _init();
        }

        AbstractPityUnit::AbstractPityUnit(
            AbstractPityUnit &parent,
            const std::string &name,
            ExecutionMode exec_mode) :
            PityTree<AbstractPityUnit>(*this, name, parent),
            _exec_mode{ exec_mode }
        {
            _init();
        }

        void AbstractPityUnit::_init()
        {
            logger_debug.set_instancename(getPath());
            if (!isRoot()) {
                // Inherit
                procUnitNr = getParent()->procUnitNr;
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
        // static
        void AbstractPityUnit::setGlobalRootDir(const std::string &dir)
        {
            AbstractPityUnit::_global_root_dir = dir;
        }

        // static
        std::string AbstractPityUnit::getGlobalRootDir()
        {
            return AbstractPityUnit::_global_root_dir;
        }


        // For:
        // RootUnit                         - "<name>"
        // ProcessUnit                      - ".../<proc>"
        // When Process as dir. parent      - ".../<proc>/name"
        // When no process as dir. parent   - ".../<proc>/.../name"
        std::string AbstractPityUnit::getPathShort() const
        {
            std::string ret;
            if (isRoot()) {
                ret = getName();
            } else {
                if (_isProcessUnit()) {
                    ret += ".../" + getName();
                } else {
                    if (&(parentingProcessUnit()) == (getParent())) {
                        ret = parentingProcessUnit().getPathShort() + "/" + getName();
                    } else {
                        ret = parentingProcessUnit().getPathShort() + "/.../" + getName();
                    }
                }
            }
            return ret;
        }

        // Every process has its own dir inside its rootUnitDir
        // All other units inherit processDir from their Root/ProcessUnit
        std::string AbstractPityUnit::processDir()
        {
            if (isRoot()) {
                return _rootUnitDir();
            } else {
                if (_isProcessUnit()) {
                    return _rootUnitDir() + getName() + "/";
                } else {
                    return getParent()->processDir();
                }
            }
        }

        // Every RootUnit has its own dir
        std::string AbstractPityUnit::_rootUnitDir()
        {
            return getGlobalRootDir() + getRoot().getName() + "/";
        }

        // Every process has its own dir inside its rootUnitDir
        // All other units inherit transportDir from their Root/ProcessUnit
        std::string AbstractPityUnit::transportDir()
        {
            if (isRoot()) {
                throw std::runtime_error("No transport dir");
            } else {
                if (_isProcessUnit()) {
                    return processDir() + "inbox/";
                } else {
                    return getParent()->transportDir();
                }
            }
        }

        void AbstractPityUnit::run()
        {
            pEpLogClass("called");
            _log_mutex = std::make_shared<fs_mutex>("log.mutex");
            _log_mutex->release();

            setenv("HOME", processDir().c_str(), true);

            if (isRoot()) {
                _initrun();
            }

            // Execute in fork and wait here until process ends
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL) { // fork
                _executeInFork(std::bind(&AbstractPityUnit::_run, this), true);
                // Execute in fork and go on, wait for process execution in the end
            } else if (_exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                _executeInFork(std::bind(&AbstractPityUnit::_run, this), false);
                // Execute as normal function
            } else if (_exec_mode == ExecutionMode::FUNCTION) {
                _run();
            } else if (_exec_mode == ExecutionMode::THREAD_PARALLEL) {
                throw std::invalid_argument(to_string(_exec_mode) + " - not implemented");
            } else if (_exec_mode == ExecutionMode::THREAD_SEQUENTIAL) {
                throw std::invalid_argument(to_string(_exec_mode) + " - not implemented");
            }

            if (isRoot()) {
                _waitChildProcesses();
            }
        }

        std::string AbstractPityUnit::to_string(bool recursive, int indent)
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
                if (!getChildren().empty()) {
                    indent++;
                    for (const auto child : getChildren()) {
                        ret += child.second.to_string(true, indent);
                    }
                    indent--;
                }
            }
            return ret;
        }

        std::string AbstractPityUnit::to_string(const ExecutionMode &emode)
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

        void AbstractPityUnit::recreateDirsRecursively()
        {
            Utils::dir_recreate(processDir());
            if (!getChildren().empty()) {
                for (const auto child : getChildren()) {
                    child.second.recreateDirsRecursively();
                }
            }
        }

        void AbstractPityUnit::registerAsTransportEndpoint()
        {
            transportEndpoints().insert({ getName(), transportDir() });
        }

        Endpoints &AbstractPityUnit::transportEndpoints()
        {
            if (isRoot()) {
                return _transport_endpoints;
            } else {
                return getRoot().transportEndpoints();
            }
        }

        void AbstractPityUnit::log(const std::string &msg) const
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

        void AbstractPityUnit::logH1(const std::string &msg) const
        {
            Adapter::pEpLog::logH1(msg, _termColor());
        }

        void AbstractPityUnit::logH2(const std::string &msg) const
        {
            Adapter::pEpLog::logH2(msg, _termColor());
        }

        void AbstractPityUnit::logH3(const std::string &msg) const
        {
            Adapter::pEpLog::logH3(msg, _termColor());
        }

        // PRIVATE ---------------------------------------------------------------------------------
        void AbstractPityUnit::_initrun()
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

        void AbstractPityUnit::_run()
        {
            logH2(_status_string("STARTING"));
            _runSelf();
            _runChildren();
        }

        void AbstractPityUnit::_runChildren() const
        {
            if (!getChildren().empty()) {
                for (const auto child : getChildren()) {
                    child.second.run();
                }
            }
        }

        void AbstractPityUnit::_executeInFork(std::function<void(void)> func, bool wait_child) const
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

        void AbstractPityUnit::_waitChildProcesses() const
        {
            int status;
            pid_t pid;
            while ((pid = wait(&status)) > 0) {
                std::string color;
                if (status == 0) {
                    color = "\033[1m\033[32m"; // Green
                } else {
                    color = "\033[1m\033[31m"; // Red
                }
                logH3(
                    color + "PROCESS [ " + std::to_string((int)pid) +
                    " ] EXITED with status code: " + std::to_string(status) +
                    Utils::to_termcol(_termColor()));
            }
        }

        bool AbstractPityUnit::_isProcessUnit() const
        {
            bool ret = false;
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL ||
                _exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                ret = true;
            }
            return ret;
        }

        const AbstractPityUnit &AbstractPityUnit::parentingProcessUnit() const
        {
            if (isRoot() || _isProcessUnit()) {
                return *this;
            } else {
                return getParent()->parentingProcessUnit();
            }
        }


        // Inherited (if null see parent recursively)
        void AbstractPityUnit::_createTransport()
        {
            registerAsTransportEndpoint();
            _transport = std::make_shared<PityTransport>(transportDir(), transportEndpoints());
        }

        // Inherited (if null see parent recursively)
        PityTransport *AbstractPityUnit::transport() const
        {
            //            pEpLogClass("called");
            PityTransport *ret = nullptr;

            if (_transport != nullptr) {
                ret = _transport.get();
            } else {
                if (!isRoot()) {
                    ret = getParent()->transport();
                }
            }
            return ret;
        }


        std::string AbstractPityUnit::_status_string(const std::string &msg) const
        {
            std::string ret;
            ret = "[ " + to_string(_exec_mode) + ":" + std::to_string(getpid()) + " ]  [ " +
                  getPathShort() + " ]  [ " + msg + " ]";
            return ret;
        }

        //static
        Utils::Color AbstractPityUnit::_colForProcUnitNr(int procUnitNr)
        {
            int nrColors = 7;
            switch (procUnitNr % nrColors) {
                case 0:
                    return Utils::Color::WHITE;
                case 1:
                    return Utils::Color::GREEN;
                case 2:
                    return Utils::Color::YELLOW;
                case 3:
                    return Utils::Color::CYAN;
                case 4:
                    return Utils::Color::BLUE;
                case 5:
                    return Utils::Color::MAGENTA;
                case 6:
                    return Utils::Color::RED;
                default:
                    return Utils::Color::WHITE;
            }
        }

        Utils::Color AbstractPityUnit::_termColor() const
        {
            return _colForProcUnitNr(procUnitNr);
        }

        void AbstractPityUnit::_logRaw(const std::string &msg) const
        {
            // fs-mutex to sync across processes
            _log_mutex->aquire();
            Adapter::pEpLog::log(msg, _termColor());
            _log_mutex->release();
        }

    } // namespace PityTest11
} // namespace pEp
