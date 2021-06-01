#include "pEpTestTree.hh"
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
        string pEpTestTree::_data_root = "./peptest";
        bool pEpTestTree::debug_log_enabled = false;

        // PUBIC CONCSTRUCTORS / FACTORY -----------------------------------------------------------
        // static
        pEpTestTree pEpTestTree::createRootNode(
            pEpTestModel& model,
            const string& name,
            const NodeFunc& test_func,
            ExecutionMode exec_mode)
        {
            pEpTestTree ret(nullptr, &model, name, test_func, exec_mode);
            return ret;
        }

        // static
        pEpTestTree pEpTestTree::createChildNode(
            pEpTestTree& parent,
            const string& name,
            const NodeFunc& test_func,
            ExecutionMode exec_mode)
        {
            pEpTestTree ret(&parent, nullptr, name, test_func, exec_mode);
            return ret;
        }

        string pEpTestTree::getNodeName() const
        {
            return _name;
        }

        string pEpTestTree::getNodePath() const
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
        std::string pEpTestTree::getNodePathShort() const
        {
            string ret;
            if (_isRootNode()) {
                ret = getNodeName();
            } else {
                if (_isProcessNode()) {
                    ret += ".../" + getNodeName();
                } else {
                    if (&(_getParentingProcessNode()) == (_parent)) {
                        ret = _getParentingProcessNode().getNodePathShort() + "/" + getNodeName();
                    } else {
                        ret = _getParentingProcessNode().getNodePathShort() + "/.../" + getNodeName();
                    }
                }
            }
            return ret;
        }

        // Inherited (if null see parent recursively)
        pEpTestModel& pEpTestTree::getModel() const
        {
            pEpLogClass("called");
            pEpTestModel* ret = nullptr;
            if (_model == nullptr) {
                ret = &(_parent->getModel());
            } else {
                ret = _model;
            }
            assert(ret != nullptr);
            // cant be null because for createChildNode() you have to provide TestNode& instance,
            // and the only other way to get one is by createRootNode() which in turn requires a TestModel&
            return *ret;
        }

        // RootNodes have their own data_dir
        // ProcessNodes have their own data_dir inside their RootNote dir (nameclash prossible)
        // All other nodes inherit data_dir from their Root/ProcessNode
        string pEpTestTree::getDataDir() const
        {
            pEpLogClass("called");
            string ret;
            if (_isRootNode()) {
                ret = getDataRoot() + getNodeName() + "/";
            } else {
                if (_parent->_exec_mode == ExecutionMode::PROCESS_SERIAL ||
                    _parent->_exec_mode == ExecutionMode::PROCESS_PARALLEL) {

                    ret = _parent->getDataDir() + getNodeName();
                } else {
                    // inherit
                    ret = _parent->getDataDir();
                }
            }
            return ret;
        }

        // static
        void pEpTestTree::setDataRoot(const string& dir)
        {
            pEpTestTree::_data_root = dir;
        }

        // static
        string pEpTestTree::getDataRoot()
        {
            return pEpTestTree::_data_root;
        }


        void pEpTestTree::run(const pEpTestTree* caller) const
        {
            pEpLogClass("called");
            // caller is never nullptr if called by another pEpTestTree
            if (caller == nullptr) {
                pEpLog::logH1("Starting pEpTestTree from node: " + getNodePathShort());
                pEpLog::log(to_string());
            }

            pEpLog::logH2(
                "[ " + to_string(_exec_mode) + " / " + getNodePathShort() + "]");
            // Execute in fork and wait here until process ends
            if (_exec_mode == ExecutionMode::PROCESS_SERIAL) { // fork
                _executeInFork(bind(&pEpTestTree::_run, this), true);
                // Execute in fork and go on, wait for process execution in the end
            } else if (_exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                _executeInFork(bind(&pEpTestTree::_run, this), false);
                // Execute as normal funciton
            } else if (_exec_mode == ExecutionMode::FUNCTION) {
                _run();
            } else if (_exec_mode == ExecutionMode::THREAD_PARALLEL) {
                throw invalid_argument(to_string(_exec_mode) + " - not implemented");
            } else if (_exec_mode == ExecutionMode::THREAD_SERIAL) {
                throw invalid_argument(to_string(_exec_mode) + " - not implemented");
            }

            if (caller == nullptr) {
                _waitChildProcesses();
            }
        }

        string pEpTestTree::to_string(bool recursive, int indent) const
        {
            string ret;
            stringstream builder;
            builder << string(indent, '\t');
            builder << getNodeName();
            builder << "[" << to_string(_exec_mode) << "]";
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

        string pEpTestTree::to_string(const ExecutionMode& emode)
        {
            switch (emode) {
                case ExecutionMode::FUNCTION:
                    return "FUNCTION";
                case ExecutionMode::PROCESS_SERIAL:
                    return "PROCESS_SERIAL";
                case ExecutionMode::PROCESS_PARALLEL:
                    return "PROCESS_PARALLEL";
                case ExecutionMode::THREAD_SERIAL:
                    return "THREAD_SERIAL";
                case ExecutionMode::THREAD_PARALLEL:
                    return "THREAD_PARALLEL";
                case ExecutionMode::INHERIT:
                    return "INHERIT";
                default:
                    return "UNDEFINED EXECUTION MODE";
            }
        }

        //Well, ok, lets just add some little convenience logging service in here, too
        void pEpTestTree::log(const string& msg) const
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

        pEpTestTree::pEpTestTree(
            pEpTestTree* const parent,
            pEpTestModel* model,
            const string& name,
            const NodeFunc& test_func,
            ExecutionMode exec_mode) :
            _parent(parent),
            _model(model), _name(_normalizeName(name)), _test_func(test_func), _exec_mode(exec_mode)
        {
            logger_debug.set_instancename(getNodePath());
            if (!_isRootNode()) {
                parent->_addChildNode(*this);
            }
        }

        void pEpTestTree::_run() const
        {
            if (_test_func) {
                _test_func(*this);
            } else {
                pEpLog::log("No function to execute");
            }
            if (!_children.empty()) {
                for (const pair<string, pEpTestTree&> child : _children) {
                    child.second.run(this);
                }
            }
        }

        void pEpTestTree::_executeInFork(function<void(void)> func, bool wait_child) const
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

        void pEpTestTree::_waitChildProcesses() const
        {
            int status;
            pid_t pid;
            while ((pid = wait(&status)) > 0) {
                pEpLog::log(
                    "process[" + std::to_string((int)pid) +
                    "] terminated with status: " + std::to_string(status));
            }
        }

        void pEpTestTree::_addChildNode(pEpTestTree& node)
        {
            _children.insert(pair<string, pEpTestTree&>(node.getNodeName(), node));
        }

        bool pEpTestTree::_isProcessNode() const
        {
            bool ret = false;
            if (_exec_mode == ExecutionMode::PROCESS_SERIAL ||
                _exec_mode == ExecutionMode::PROCESS_PARALLEL) {
                ret = true;
            }
            return ret;
        }

        bool pEpTestTree::_isRootNode() const
        {
            bool ret = false;
            if (_parent == nullptr) {
                ret = true;
            }
            return ret;
        }

        const pEpTestTree& pEpTestTree::_getRootNode() const
        {
            const pEpTestTree* ret = nullptr;
            if (!_isRootNode()) {
                ret = &(_parent->_getRootNode());
            } else {
                ret = this;
            }
            assert(ret != nullptr);
            // cant be null because for createChildNode() you need to provide a TestNode& and
            // the only other way is using createRootNode() which has parent == nullptr
            return *ret;
        }

        const pEpTestTree& pEpTestTree::_getParentingProcessNode() const
        {
            if (_isRootNode() || _isProcessNode()) {
                return *this;
            } else {
                return _parent->_getParentingProcessNode();
            }
        }

        // name is alphanumeric only (everything else will be replaced by an underscore)
        string pEpTestTree::_normalizeName(string name) const
        {
            replace_if(
                name.begin(),
                name.end(),
                [](char c) -> bool { return !isalnum(c); },
                '_');

            return name;
        }

        void pEpTestTree::_data_dir_create()
        {
            //            Utils::dir_create(getDataDir());
            pEpLog::log("creating dir:" + getDataRoot());
        }

        void pEpTestTree::_data_dir_delete()
        {
            try {
                Utils::path_delete_all(getDataRoot());
            } catch (const exception& e) {
                pEpLog::log("DistTest: - could not delete data dir: " + getDataRoot());
            }
        }

        void pEpTestTree::_data_dir_recreate()
        {
            _data_dir_delete();
            _data_dir_create();
        };
    } // namespace Test
} // namespace pEp
