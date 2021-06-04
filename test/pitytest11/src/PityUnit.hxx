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

using namespace pEp::Adapter::pEpLog;

namespace pEp {
    namespace PityTest11 {
        template<class T>
        std::string PityUnit<T>::_global_root_dir = "./pitytest_data/";
        template<class T>
        bool PityUnit<T>::debug_log_enabled = false;

        // CONCSTRUCTORS
        template<class T>
        PityUnit<T>::PityUnit(
            PityUnit* const parent,
            const std::string& name,
            const NodeFunc test_func,
            T* model,
            ExecutionMode exec_mode) :
            _parent(parent),
            _model(model), _name(_normalizeName(name)), _test_func(test_func), _exec_mode(exec_mode)
        {
            logger_debug.set_instancename(getNodePath());
            if (!_isRootNode()) {
                parent->_addChildNode(*this);
            }
        }

        template<class T>
        std::string PityUnit<T>::getNodeName() const
        {
            return _name;
        }

        template<class T>
        std::string PityUnit<T>::getNodePath() const
        {
            std::string ret;

            if (!_isRootNode()) {
                ret = _parent->getNodePath() + "/" + getNodeName();
            } else {
                ret = getNodeName();
            }
            return ret;
        }

        // For:
        // RootNode                         - "<name>"
        // ProcessNode                      - ".../<proc>"
        // When Process as dir. parent      - ".../<proc>/name"
        // When no process as dir. parent   - ".../<proc>/.../name"
        template<class T>
        std::string PityUnit<T>::getNodePathShort() const
        {
            std::string ret;
            if (_isRootNode()) {
                ret = getNodeName();
            } else {
                if (_isProcessNode()) {
                    ret += ".../" + getNodeName();
                } else {
                    if (&(_parentingProcessNode()) == (_parent)) {
                        ret = _parentingProcessNode().getNodePathShort() + "/" + getNodeName();
                    } else {
                        ret = _parentingProcessNode().getNodePathShort() + "/.../" + getNodeName();
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
                if (!_isRootNode()) {
                    ret = _parent->getModel();
                }
            }
            return ret;
        }

        // Every RootNode has its own dir
        template<class T>
        std::string PityUnit<T>::rootNodeDir() const
        {
            return getGlobalRootDir() + _rootNode().getNodeName() + "/";
        }

        // Every process has its own dir inside its rootNodeDir
        // All other nodes inherit processDir from their Root/ProcessNode
        template<class T>
        std::string PityUnit<T>::processDir() const
        {
            if (_isRootNode()) {
                return rootNodeDir();
            } else {
                if (_isProcessNode()) {
                    return rootNodeDir() + getNodeName() + "/";
                } else {
                    return _parent->processDir();
                }
            }
        }

        template<class T>
        void PityUnit<T>::setExecutionMode(ExecutionMode mode)
        {
            _exec_mode = mode;
        }

        // static
        template<>
        void PityUnit<void>::setGlobalRootDir(const std::string& dir)
        {
            PityUnit::_global_root_dir = dir;
        }

        // static
        template<class T>
        std::string PityUnit<T>::getGlobalRootDir()
        {
            return PityUnit::_global_root_dir;
        }

        template<class T>
        void PityUnit<T>::_init() const
        {
            _ensureDir(getGlobalRootDir());
            _recreateDir(processDir());
            if (!_children.empty()) {
                for (const std::pair<std::string, PityUnit<T>&> child : _children) {
                    _recreateDir(child.second.processDir());
                }
            }
        }

        template<class T>
        void PityUnit<T>::run() const
        {
            pEpLogClass("called");
            _init();
            // caller is never nullptr if called by another PityUnit
            if (_isRootNode()) {
                logH1("Starting PityUnit from node: " + getNodePathShort());
                std::cout << to_string() << std::endl;
            }

            // Execute in fork and wait here until process ends
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL) { // fork
                logH2(_status_string("RUNNING"));
                _executeInFork(std::bind(&PityUnit::_run, this), true);
                // Execute in fork and go on, wait for process execution in the end
            } else if (_exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                logH2(_status_string("RUNNING"));
                _executeInFork(std::bind(&PityUnit::_run, this), false);
                // Execute as normal funciton
            } else if (_exec_mode == ExecutionMode::FUNCTION) {
                logH3(_status_string("RUNNING"));
                _run();
            } else if (_exec_mode == ExecutionMode::THREAD_PARALLEL) {
                throw std::invalid_argument(to_string(_exec_mode) + " - not implemented");
            } else if (_exec_mode == ExecutionMode::THREAD_SEQUENTIAL) {
                throw std::invalid_argument(to_string(_exec_mode) + " - not implemented");
            }

            if (_isRootNode()) {
                _waitChildProcesses();
            }
        }

        template<class T>
        std::string PityUnit<T>::to_string(bool recursive, int indent) const
        {
            std::string ret;
            std::stringstream builder;
            builder << std::string(indent * 4, ' ');

            builder << getNodeName();
            builder << " [ ";
            builder << to_string(_exec_mode) << " - ";
            builder << "\"" << processDir() << "\"";
            builder << " ]";
            builder << std::endl;
            ret = builder.str();

            if (recursive) {
                indent++;
                for (const std::pair<std::string, const PityUnit&> child : _children) {
                    ret += child.second.to_string(true, indent);
                }
                indent--;
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

        //Well, ok, lets just add some little convenience logging service in here, too
        template<class T>
        void PityUnit<T>::log(const std::string& msg) const
        {
            std::stringstream builder;
            builder << "[";
            builder << std::to_string(getpid());
            builder << " - ";
            builder << getNodePathShort();
            builder << "] -  ";
            builder << msg;
            builder << std::endl;
            std::cout << builder.str();
        }

        // PRIVATE ---------------------------------------------------------------------------------

        template<class T>
        void PityUnit<T>::_run() const
        {
            _runSelf();
            _runChildren();
        }

        template<class T>
        void PityUnit<T>::_runSelf() const
        {
            if (_test_func != nullptr) {
                try {
                    _test_func(*this);
                    logH3(_status_string("SUCCESS"));
                } catch (const std::exception& e) {
                    log("reason: " + std::string(e.what()));
                    logH3(_status_string("FAILED"));
                }
            } else {
                log("No function to execute");
            }
        }

        template<class T>
        void PityUnit<T>::_runChildren() const
        {
            if (!_children.empty()) {
                for (const std::pair<std::string, PityUnit&> child : _children) {
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
        void PityUnit<T>::_addChildNode(PityUnit& node)
        {
            _children.insert(std::pair<std::string, PityUnit&>(node.getNodeName(), node));
        }

        template<class T>
        bool PityUnit<T>::_isProcessNode() const
        {
            bool ret = false;
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL ||
                _exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                ret = true;
            }
            return ret;
        }

        template<class T>
        bool PityUnit<T>::_isRootNode() const
        {
            if (_parent == nullptr) {
                return true;
            } else {
                return false;
            }
        }

        template<class T>
        const PityUnit<T>& PityUnit<T>::_rootNode() const
        {
            const PityUnit* ret = nullptr;
            if (!_isRootNode()) {
                ret = &(_parent->_rootNode());
            } else {
                ret = this;
            }
            assert(ret != nullptr);
            // cant be null because for createChildNode() you need to provide a TestNode& and
            // the only other way is using createRootNode() which has parent == nullptr
            return *ret;
        }

        template<class T>
        const PityUnit<T>& PityUnit<T>::_parentingProcessNode() const
        {
            if (_isRootNode() || _isProcessNode()) {
                return *this;
            } else {
                return _parent->_parentingProcessNode();
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
                  getNodePathShort() + " ]  [ " + msg + " ]";
            return ret;
        }

        template<class T>
        void PityUnit<T>::_ensureDir(const std::string& path) const
        {
            if (!Utils::path_exists(path)) {
                log("creating dir:" + path);
                Utils::dir_create(path);
            }
        }

        template<class T>
        void PityUnit<T>::_recreateDir(const std::string& path) const
        {
            if (Utils::path_exists(path)) {
                try {
                    log("deleting dir:" + path);
                    Utils::path_delete_all(path);
                } catch (const std::exception& e) {
                    log("PityUnit: - could not delete data dir: " + getGlobalRootDir());
                }
            }
            log("creating dir:" + path);
            Utils::dir_create(path);
        }


        template<class T>
        void PityUnit<T>::_data_dir_create()
        {
            //            Utils::dir_create(dataDir());
            log("creating dir:" + getGlobalRootDir());
        }

        template<class T>
        void PityUnit<T>::_data_dir_delete()
        {
            try {
                //                Utils::path_delete_all(getGlobalRootDir());
            } catch (const std::exception& e) {
                log("DistTest: - could not delete data dir: " + getGlobalRootDir());
            }
        }

        template<class T>
        void PityUnit<T>::_data_dir_recreate()
        {
            _data_dir_delete();
            _data_dir_create();
        };
    } // namespace PityTest11
} // namespace pEp

#endif // PITYTEST_PITYUNIT_HXX