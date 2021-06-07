// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYUNIT_HXX
#define PITYTEST_PITYUNIT_HXX

#include "../../../src/std_utils.hh"
//#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <functional>
#include <algorithm>
#include <sstream>
#include <exception>

//using namespace pEp::Adapter::pEpLog;

namespace pEp {
    namespace PityTest11 {
        // static
        template<class T>
        std::string PityUnit<T>::_global_root_dir = "./pitytest_data/";
        // static
        template<class T>
        bool PityUnit<T>::debug_log_enabled = false;
        // static
        template<class T>
        int PityUnit<T>::procUnitsCount = 0;

        // CONSTRUCTOR
        template<class T>
        PityUnit<T>::PityUnit(
            PityUnit<T>* const parent,
            const std::string& name,
            const std::function<void(const PityUnit&)> test_func,
            T* model,
            ExecutionMode exec_mode) :
            _parent{ parent },
            _model{ model }, _name{ _normalizeName(name) }, _test_func{ test_func }, _exec_mode{
                exec_mode
            }
        {
            logger_debug.set_instancename(getPath());
            if (!_isRootUnit()) {
                parent->_addChildUnit(*this);
                // Inherit
                procUnitNr = _parent->procUnitNr;
                //Or update if procUnit
                if (_isProcessUnit()) {
                    procUnitsCount++;
                    procUnitNr = procUnitsCount;
                }
            } else {
                procUnitNr = procUnitsCount;
            }
        }

        template<class T>
        std::string PityUnit<T>::getName() const
        {
            return _name;
        }

        template<class T>
        std::string PityUnit<T>::getPath() const
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
        template<class T>
        std::string PityUnit<T>::getPathShort() const
        {
            std::string ret;
            if (_isRootUnit()) {
                ret = getName();
            } else {
                if (_isProcessUnit()) {
                    ret += ".../" + getName();
                } else {
                    if (&(_parentingProcessUnit()) == (_parent)) {
                        ret = _parentingProcessUnit().getPathShort() + "/" + getName();
                    } else {
                        ret = _parentingProcessUnit().getPathShort() + "/.../" + getName();
                    }
                }
            }
            return ret;
        }

        // Inherited (if null see parent recursively)
        template<class T>
        T* PityUnit<T>::getModel() const
        {
            pEpLogClass("called");
            T* ret = nullptr;

            if (_model != nullptr) {
                ret = _model;
            } else {
                if (!_isRootUnit()) {
                    ret = _parent->getModel();
                }
            }
            return ret;
        }

        // Every RootUnit has its own dir
        template<class T>
        std::string PityUnit<T>::_rootUnitDir() const
        {
            return getGlobalRootDir() + _rootUnit().getName() + "/";
        }

        // Every process has its own dir inside its rootUnitDir
        // All other units inherit processDir from their Root/ProcessUnit
        template<class T>
        std::string PityUnit<T>::processDir() const
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

        //        template<class T>
        //        void PityUnit<T>::setExecutionMode(ExecutionMode mode)
        //        {
        //            _exec_mode = mode;
        //        }

        // static
        template<class T>
        void PityUnit<T>::setGlobalRootDir(const std::string& dir)
        {
            PityUnit<T>::_global_root_dir = dir;
        }

        // static
        template<class T>
        std::string PityUnit<T>::getGlobalRootDir()
        {
            return PityUnit::_global_root_dir;
        }

        template<class T>
        void PityUnit<T>::run() const
        {
            pEpLogClass("called");
            // caller is never nullptr if called by another PityUnit
            if (_isRootUnit()) {
                _init();
            }

            // Execute in fork and wait here until process ends
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL) { // fork
                _executeInFork(std::bind(&PityUnit<T>::_run, this), true);
                // Execute in fork and go on, wait for process execution in the end
            } else if (_exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                _executeInFork(std::bind(&PityUnit<T>::_run, this), false);
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

        template<class T>
        std::string PityUnit<T>::to_string(bool recursive, int indent) const
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
                    for (const std::pair<std::string, const PityUnit<T>&> child : _children) {
                        ret += child.second.to_string(true, indent);
                    }
                    indent--;
                }
            }
            return ret;
        }

        template<class T>
        std::string PityUnit<T>::to_string(const ExecutionMode& emode)
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

        template<class T>
        void PityUnit<T>::log(const std::string& msg) const
        {
            std::stringstream builder;
            builder << "[";
            builder << std::to_string(getpid());
            builder << " - ";
            builder << getPathShort();
            builder << "] -  ";
            builder << msg;

            logRaw(builder.str());
        }


        template<class T>
        void PityUnit<T>::logH1(const std::string& msg) const
        {
            Adapter::pEpLog::logH1(msg, _termColor());
        }

        template<class T>
        void PityUnit<T>::logH2(const std::string& msg) const
        {
            Adapter::pEpLog::logH2(msg, _termColor());
        }

        template<class T>
        void PityUnit<T>::logH3(const std::string& msg) const
        {
            Adapter::pEpLog::logH3(msg, _termColor());
        }


        // PRIVATE ---------------------------------------------------------------------------------
        template<class T>
        void PityUnit<T>::_init() const
        {
            logH1("PityTest Starting...");
            logRaw("RootUnit: " + getPathShort());
            logRaw("GlobalRootDir: " + getGlobalRootDir());
            logRaw("\nTestTree");
            logRaw("--------");
            logRaw(to_string());

            logH3("INIT");
            _ensureDir(getGlobalRootDir());
            recreateDirsRecursively();
            //            if (!_children.empty()) {
            //                for (const std::pair<std::string, PityUnit<T>&> child : _children) {
            //                    _recreateDir(child.second.processDir());
            //                }
            //            }
            logH3("INIT DONE");
        }


        template<class T>
        void PityUnit<T>::_run() const
        {
            logH2(_status_string("STARTING"));
            _runSelf();
            _runChildren();
        }

        template<class T>
        void PityUnit<T>::_runSelf() const
        {
            if (_test_func != nullptr) {
                try {
                    _test_func(*this);
                    logH3(_status_string("\033[1m\033[32mSUCCESS" + Utils::to_termcol(_termColor())));
                } catch (const std::exception& e) {
                    logRaw("reason: " + std::string(e.what()));
                    logH3(_status_string("\033[1m\033[31mFAILED" + Utils::to_termcol(_termColor())
                                         ));
                }
            } else {
                logRaw("No function to execute");
            }
        }

        template<class T>
        void PityUnit<T>::_runChildren() const
        {
            if (!_children.empty()) {
                for (const std::pair<std::string, PityUnit<T>&> child : _children) {
                    child.second.run();
                }
            }
        }

        template<class T>
        void PityUnit<T>::_executeInFork(std::function<void(void)> func, bool wait_child) const
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

        template<class T>
        void PityUnit<T>::_waitChildProcesses() const
        {
            int status;
            pid_t pid;
            while ((pid = wait(&status)) > 0) {
                pEpLogClass(
                    "process[" + std::to_string((int)pid) +
                    "] terminated with status: " + std::to_string(status));
            }
        }

        template<class T>
        void PityUnit<T>::_addChildUnit(PityUnit<T>& unit)
        {
            _children.insert(std::pair<std::string, PityUnit<T>&>(unit.getName(), unit));
        }

        template<class T>
        bool PityUnit<T>::_isProcessUnit() const
        {
            bool ret = false;
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL ||
                _exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                ret = true;
            }
            return ret;
        }

        template<class T>
        bool PityUnit<T>::_isRootUnit() const
        {
            if (_parent == nullptr) {
                return true;
            } else {
                return false;
            }
        }

        template<class T>
        const PityUnit<T>& PityUnit<T>::_rootUnit() const
        {
            const PityUnit<T>* ret = nullptr;
            if (!_isRootUnit()) {
                ret = &(_parent->_rootUnit());
            } else {
                ret = this;
            }
            assert(ret != nullptr);
            // cant be null because for createChildUnit() you need to provide a TestUnit& and
            // the only other way is using createRootUnit() which has parent == nullptr
            return *ret;
        }

        template<class T>
        const PityUnit<T>& PityUnit<T>::_parentingProcessUnit() const
        {
            if (_isRootUnit() || _isProcessUnit()) {
                return *this;
            } else {
                return _parent->_parentingProcessUnit();
            }
        }

        // name is alphanumeric only (everything else will be replaced by an underscore)
        template<class T>
        std::string PityUnit<T>::_normalizeName(std::string name) const
        {
            replace_if(
                name.begin(),
                name.end(),
                [](char c) -> bool { return !isalnum(c); },
                '_');

            return name;
        }

        template<class T>
        std::string PityUnit<T>::_status_string(const std::string& msg) const
        {
            std::string ret;
            ret = "[ " + to_string(_exec_mode) + ":" + std::to_string(getpid()) + " ]  [ " +
                  getPathShort() + " ]  [ " + msg + " ]";
            return ret;
        }


        template<class T>
        Utils::Color PityUnit<T>::_colForProcUnitNr(int procUnitNr) const
        {
            switch (procUnitNr) {
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

        template<class T>
        Utils::Color PityUnit<T>::_termColor() const
        {
            return _colForProcUnitNr(procUnitNr);
        }

        template<class T>
        void PityUnit<T>::logRaw(const std::string& msg) const
        {
            Adapter::pEpLog::log(msg, _termColor());
        }

        template<class T>
        void PityUnit<T>::_ensureDir(const std::string& path) const
        {
            if (!Utils::path_exists(path)) {
                logRaw("creating dir:" + path);
                Utils::dir_create(path);
            }
        }

        template<class T>
        void PityUnit<T>::_recreateDir(const std::string& path) const
        {
            if (Utils::path_exists(path)) {
                try {
                    logRaw("deleting dir:" + path);
                    Utils::path_delete_all(path);
                } catch (const std::exception& e) {
                    logRaw("PityUnit: - could not delete data dir: " + getGlobalRootDir());
                }
            }
            logRaw("creating dir:" + path);
            Utils::dir_create(path);
        }

        template<class T>
        void PityUnit<T>::recreateDirsRecursively() const
        {
            _recreateDir(processDir());
            if (!_children.empty()) {
                for (const std::pair<std::string, PityUnit<T>&> child : _children) {
                    child.second.recreateDirsRecursively();
                }
            }
        }

    } // namespace PityTest11
} // namespace pEp

#endif // PITYTEST_PITYUNIT_HXX