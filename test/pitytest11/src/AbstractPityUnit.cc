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
        int AbstractPityUnit::_procUnitsCount = 0;

        AbstractPityUnit::AbstractPityUnit(const std::string &name, ExecutionMode exec_mode) :
            PityTree<AbstractPityUnit>(*this, name), _exec_mode{ exec_mode }, _procUnitNr{ 0 }
        {
            _init();
        }

        AbstractPityUnit::AbstractPityUnit(
            AbstractPityUnit &parent,
            const std::string &name,
            ExecutionMode exec_mode) :
            PityTree<AbstractPityUnit>(*this, name, parent),
            _exec_mode{ exec_mode }, _procUnitNr{ 0 }
        {
            _init();
        }

        AbstractPityUnit::AbstractPityUnit(const AbstractPityUnit &rhs, AbstractPityUnit &self) :
            PityTree<AbstractPityUnit>(rhs, self)
        {
            _procUnitNr = rhs._procUnitNr;
            _exec_mode = rhs._exec_mode;
            _transport = rhs._transport;
            _transport_endpoints = rhs._transport_endpoints;
            _init();
        }

        AbstractPityUnit &AbstractPityUnit::operator=(const AbstractPityUnit &rhs)
        {
            _procUnitNr = rhs._procUnitNr;
            _exec_mode = rhs._exec_mode;
            _transport = rhs._transport;
            _transport_endpoints = rhs._transport_endpoints;
            return *this;
        }


        void AbstractPityUnit::_init()
        {
            _log_mutex = std::make_shared<fs_mutex>("log.mutex");
            _log_mutex->release();
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

        void AbstractPityUnit::_initProcUnitNrRecurse()
        {
            if (!isRoot()) {
                // Inherit
                _procUnitNr = getParent()->_procUnitNr;
                //Or update if procUnit
                if (_isProcessUnit()) {
                    _procUnitsCount++;
                    _procUnitNr = _procUnitsCount;
                }
            } else {
                _procUnitNr = _procUnitsCount;
            }

            // Recurse
            for (const auto &chld : getChildRefs()) {
                chld.second._initProcUnitNrRecurse();
            }
        }

        void AbstractPityUnit::_initTransportRecurse()
        {
            logger_debug.set_instancename(getPath());
            if (!isRoot()) {
                if (_isProcessUnit()) {
                    _createTransport();
                }
            }

            // Recurse
            for (const auto &chld : getChildRefs()) {
                chld.second._initTransportRecurse();
            }
        }

        void AbstractPityUnit::_initDirsRecursive()
        {
            Utils::dir_recreate(processDir());

            // Recurse
            for (const auto &child : getChildRefs()) {
                child.second._initDirsRecursive();
            }
        }


        void AbstractPityUnit::run(bool init_tree)
        {
            pEpLogClass("called");

            if (init_tree) {
                logH1("PityTest Starting...");
                _logRaw("RootUnit: " + getPath());
                _logRaw("GlobalRootDir: " + getGlobalRootDir());

                _logRaw("Ensuring GlobalRootDir...");
                Utils::dir_ensure(getGlobalRootDir());

                _logRaw("Recreating process dirs recursively...");
                _initDirsRecursive();

                _logRaw("Initializing Transport recursively...");
                _initTransportRecurse();

                _logRaw("\n\nTestTree");
                _logRaw("--------");
                _logRaw(to_string() + "\n");
                _procUnitsCount = 0;
                _initProcUnitNrRecurse();
            }


            // TODO: hack
            setenv("HOME", processDir().c_str(), true);

            // Execute in fork and wait here until process ends
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL) { // fork
                _executeInFork(std::bind(&AbstractPityUnit::_runRecurse, this), true);
                // Execute in fork and go on, wait for process execution in the end
            } else if (_exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                _executeInFork(std::bind(&AbstractPityUnit::_runRecurse, this), false);
                // Execute as normal function
            } else if (_exec_mode == ExecutionMode::FUNCTION) {
                _runRecurse();
            } else if (_exec_mode == ExecutionMode::THREAD_PARALLEL) {
                throw std::invalid_argument(to_string(_exec_mode) + " - not implemented");
            } else if (_exec_mode == ExecutionMode::THREAD_SEQUENTIAL) {
                throw std::invalid_argument(to_string(_exec_mode) + " - not implemented");
            }

            if (init_tree) {
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
                if (!getChildRefs().empty()) {
                    indent++;
                    for (const auto child : getChildRefs()) {
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
            Adapter::pEpLog::logH1(msg, _color());
        }

        void AbstractPityUnit::logH2(const std::string &msg) const
        {
            Adapter::pEpLog::logH2(msg, _color());
        }

        void AbstractPityUnit::logH3(const std::string &msg) const
        {
            Adapter::pEpLog::logH3(msg, _color());
        }

        // PRIVATE ---------------------------------------------------------------------------------
        void AbstractPityUnit::_runRecurse()
        {
            logH2(_status_string("STARTING"));
            _runSelf();
            if (!getChildRefs().empty()) {
                for (const auto child : getChildRefs()) {
                    child.second.run(false);
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
                    Utils::to_termcol(_color()));
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

        Utils::Color AbstractPityUnit::_color() const
        {
            return _colForProcUnitNr(_procUnitNr);
        }

        void AbstractPityUnit::_logRaw(const std::string &msg) const
        {
            // fs-mutex to sync across processes
            _log_mutex->aquire();
            Adapter::pEpLog::log(msg, _color());
            _log_mutex->release();
        }
    } // namespace PityTest11
} // namespace pEp
