// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HXX
#define LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HXX

#include "../../../src/std_utils.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <functional>
#include <algorithm>
#include <sstream>


using namespace std;
using namespace pEp::Adapter;
using namespace pEp::Utils;

namespace pEp {
    namespace Test {
        template<class T>
        string pEpTestTree<T>::_global_root_dir = "./peptest";
        template<class T>
        bool pEpTestTree<T>::debug_log_enabled = false;

        // PUBLIC CONCSTRUCTORS / FACTORY -----------------------------------------------------------
        template<class T>
        pEpTestTree<T>::pEpTestTree(
            pEpTestTree* const parent,
            const string& name,
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
        string pEpTestTree<T>::getNodeName() const
        {
            return _name;
        }

        template<class T>
        string pEpTestTree<T>::getNodePath() const
        {
            pEpLogClass("called");
            string ret;

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
        std::string pEpTestTree<T>::getNodePathShort() const
        {
            string ret;
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
        T* pEpTestTree<T>::getModel() const
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

        // RootNodes have their own data_dir
        // ProcessNodes have their own data_dir inside their RootNote dir (nameclash prossible)
        // All other nodes inherit data_dir from their Root/ProcessNode
        //        string pEpTestTree::dataDir() const
        //        {
        //            pEpLogClass("called");
        //            string ret;
        //            if(!_isRootNode()) {
        //                ret = getRootNodesDir() + _getRootNode().getNodeName() + "/" + _getParentingProcessNode().getNodeName() + "/";
        //            } else {
        //                ret = getRootNodesDir() + _getRootNode().getNodeName() + "/";
        //            }
        //            return ret;
        //        }
        //

        // Every RootNode has its own dir
        template<class T>
        string pEpTestTree<T>::rootNodeDir() const
        {
            return getGlobalRootDir() + _rootNode().getNodeName() + "/";
        }

        // Every process has its own dir inside its rootNodeDir
        // The Root node has its own processDir for future use
        template<class T>
        string pEpTestTree<T>::processDir() const
        {
            if (_isRootNode()) {
                return rootNodeDir() + "rootnode_data";
            } else {
                if (&_parentingProcessNode() == &_rootNode()) {
                    return rootNodeDir() + getNodeName() + "/";
                } else {
                    return rootNodeDir() + _parentingProcessNode().getNodeName() + "/";
                };
            }
        }

        template<class T>
        void pEpTestTree<T>::setExecutionMode(ExecutionMode mode)
        {
            _exec_mode = mode;
        }

        // static
        template<>
        void pEpTestTree<void>::setGlobalRootDir(const string& dir)
        {
            pEpTestTree::_global_root_dir = dir;
        }

        // static
        template<class T>
        string pEpTestTree<T>::getGlobalRootDir()
        {
            return pEpTestTree::_global_root_dir + "/";
        }

        template<class T>
        void pEpTestTree<T>::run() const
        {
            pEpLogClass("called");
            // caller is never nullptr if called by another pEpTestTree
            if (_isRootNode()) {
                pEpLog::logH1("Starting pEpTestTree from node: " + getNodePathShort());
                pEpLog::log(to_string());
            }

            // Execute in fork and wait here until process ends
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL) { // fork
                pEpLog::logH2("[ " + to_string(_exec_mode) + " / " + getNodePathShort() + "]");
                _executeInFork(bind(&pEpTestTree::_run, this), true);
                // Execute in fork and go on, wait for process execution in the end
            } else if (_exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                pEpLog::logH2("[ " + to_string(_exec_mode) + " / " + getNodePathShort() + "]");
                _executeInFork(bind(&pEpTestTree::_run, this), false);
                // Execute as normal funciton
            } else if (_exec_mode == ExecutionMode::FUNCTION) {
                pEpLog::logH3("[ " + to_string(_exec_mode) + " / " + getNodePathShort() + "]");
                _run();
            } else if (_exec_mode == ExecutionMode::THREAD_PARALLEL) {
                throw invalid_argument(to_string(_exec_mode) + " - not implemented");
            } else if (_exec_mode == ExecutionMode::THREAD_SEQUENTIAL) {
                throw invalid_argument(to_string(_exec_mode) + " - not implemented");
            }

            _waitChildProcesses();
        }

        template<class T>
        string pEpTestTree<T>::to_string(bool recursive, int indent) const
        {
            string ret;
            stringstream builder;
            builder << string(indent * 4, ' ');

            builder << getNodeName();
            builder << " [ ";
            builder << to_string(_exec_mode) << " - ";
            builder << "\"" << processDir() << "\"";
            builder << " ]";
            builder << endl;
            ret = builder.str();

            if (recursive) {
                indent++;
                for (const pair<string, const pEpTestTree&> child : _children) {
                    ret += child.second.to_string(true, indent);
                }
                indent--;
            }
            return ret;
        }

        template<class T>
        string pEpTestTree<T>::to_string(const ExecutionMode& emode)
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
        void pEpTestTree<T>::log(const string& msg) const
        {
            stringstream builder;
            builder << "[";
            builder << std::to_string(getpid());
            builder << " - ";
            builder << getNodePathShort();
            builder << "] -  ";
            builder << msg;
            builder << endl;
            cout << builder.str();
        }

        // PRIVATE ---------------------------------------------------------------------------------

        template<class T>
        void pEpTestTree<T>::_run() const
        {
            _runSelf();
            _runChildren();
        }

        template<class T>
        void pEpTestTree<T>::_runSelf() const
        {
            if (_test_func != nullptr) {
                _test_func(*this);
            } else {
                pEpLog::log("No function to execute");
            }
        }

        template<class T>
        void pEpTestTree<T>::_runChildren() const
        {
            if (!_children.empty()) {
                for (const pair<string, pEpTestTree&> child : _children) {
                    child.second.run();
                }
            }
        }

        template<class T>
        void pEpTestTree<T>::_executeInFork(function<void(void)> func, bool wait_child) const
        {
            pid_t pid;
            pid = fork();
            if (pid == pid_t(0)) {
                func();
                exit(0);
            } else if (pid < pid_t(0)) {
                throw runtime_error("Error forking");
            }
            if (wait_child) {
                _waitChildProcesses();
            }
        }

        template<class T>
        void pEpTestTree<T>::_waitChildProcesses() const
        {
            int status;
            pid_t pid;
            while ((pid = wait(&status)) > 0) {
                pEpLog::log(
                    "process[" + std::to_string((int)pid) +
                    "] terminated with status: " + std::to_string(status));
            }
        }

        template<class T>
        void pEpTestTree<T>::_addChildNode(pEpTestTree& node)
        {
            _children.insert(pair<string, pEpTestTree&>(node.getNodeName(), node));
        }

        template<class T>
        bool pEpTestTree<T>::_isProcessNode() const
        {
            bool ret = false;
            if (_exec_mode == ExecutionMode::PROCESS_SEQUENTIAL ||
                _exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                ret = true;
            }
            return ret;
        }

        template<class T>
        bool pEpTestTree<T>::_isRootNode() const
        {
            if (_parent == nullptr) {
                return true;
            } else {
                return false;
            }
        }

        template<class T>
        const pEpTestTree<T>& pEpTestTree<T>::_rootNode() const
        {
            const pEpTestTree* ret = nullptr;
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
        const pEpTestTree<T>& pEpTestTree<T>::_parentingProcessNode() const
        {
            if (_isRootNode() || _isProcessNode()) {
                return *this;
            } else {
                return _parent->_parentingProcessNode();
            }
        }

        // name is alphanumeric only (everything else will be replaced by an underscore)
        template<class T>
        string pEpTestTree<T>::_normalizeName(string name) const
        {
            replace_if(
                name.begin(),
                name.end(),
                [](char c) -> bool { return !isalnum(c); },
                '_');

            return name;
        }

        template<class T>
        void pEpTestTree<T>::_data_dir_create()
        {
            //            Utils::dir_create(dataDir());
            pEpLog::log("creating dir:" + getGlobalRootDir());
        }

        template<class T>
        void pEpTestTree<T>::_data_dir_delete()
        {
            try {
                Utils::path_delete_all(getGlobalRootDir());
            } catch (const exception& e) {
                pEpLog::log("DistTest: - could not delete data dir: " + getGlobalRootDir());
            }
        }

        template<class T>
        void pEpTestTree<T>::_data_dir_recreate()
        {
            _data_dir_delete();
            _data_dir_create();
        };
    } // namespace Test
} // namespace pEp

#endif // LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HXX