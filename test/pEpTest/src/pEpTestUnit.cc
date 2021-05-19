#include "pEpTestUnit.hh"
#include "../../../src/std_utils.hh"
#include "../../framework/utils.hh"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <functional>

using namespace std;

namespace pEp {
    namespace Test {
        bool pEpTestUnit::log_enabled = true;
        string pEpTestUnit::data_dir = "./";
        pEpTestUnit::ExecutionMode pEpTestUnit::exec_mode_default = pEpTestUnit::ExecutionMode::FUNCTION;

        // RootNode factory
        //static
        pEpTestUnit pEpTestUnit::createRootNode(
            pEpTestModel& model,
            const std::string& name,
            const TestEntryFunc& main_f,
            ExecutionMode emode)
        {
            pEpLog("called");
            pEpTestUnit ret(nullptr, &model, name, main_f, emode);
            return ret;
        }

        // ChildNode factory
        //static
        pEpTestUnit pEpTestUnit::createChildNode(
            pEpTestUnit& parent,
            const std::string& name,
            const TestEntryFunc& main_f,
            ExecutionMode emode)
        {
            pEpLog("called");
            pEpTestUnit ret(&parent, nullptr, name, main_f, emode);
            return ret;
        }

        //private
        pEpTestUnit::pEpTestUnit(
            pEpTestUnit* const parent,
            pEpTestModel* model,
            const std::string& name,
            const TestEntryFunc& main_f,
            ExecutionMode emode) :
            parent(parent),
            model(model), name(name), main_func(main_f), exec_mode(emode)
        {
            if (parent != nullptr) {
                parent->addChildNode(*this);
            }
            logger.set_instancename(getFQName());
            //            data_dir_recreate();
        }


        //static
        void pEpTestUnit::setDefaultDataDir(const std::string& dir)
        {
            pEpTestUnit::data_dir = dir;
        }

        //static
        std::string pEpTestUnit::getDataDir()
        {
            return pEpTestUnit::data_dir;
        }

        //        void TestNode::init() const
        //        {
        //            pEpLogClassH2("DistTest - init");
        //            for (const pair<string, TestNode> elem : testnodes) {
        //                string home_dir = getHomeDir(elem.second);
        //                pEpLogClass("creating home dir for '" + elem.second.getName() + "' - " + home_dir);
        //                mkdir(home_dir.c_str(), 0770);
        //            }
        //        }


        void pEpTestUnit::waitChildProcesses() const
        {
            pEpLogClass("called");
            int status;
            pid_t pid;
            while ((pid = wait(&status)) > 0) {
                pEpLogClass(
                    "PID: " + std::to_string((int)pid) + " exit status: " + std::to_string(status));
            }
        }

        void pEpTestUnit::addChildNode(const pEpTestUnit& node)
        {
            children.insert(pair<string, pEpTestUnit>(node.getName(), node));
        }

        string pEpTestUnit::getName() const
        {
            return name;
        }

        void pEpTestUnit::run(const pEpTestUnit* caller) const
        {
            //            pEpLogClass("called");
            //            init();
            executeSelf();
            executeChildren();
            // if called by another TestNode, caller will not be nullptr, so caller will clean up
            // if caller is mullptr, there is nobody who will do the cleanup, so we have to.
            if (caller == nullptr) {
                waitChildProcesses();
            }
        }
        //private
        void pEpTestUnit::executeSelf() const
        {
            //            pEpLogClass("called");
            if (main_func) {
                pEpLogClass(string("Starting as: " + to_string(getEffectiveExecutionMode())));
                // Execute in fork and go on, wait for process execution in the edn
                if (getEffectiveExecutionMode() == ExecutionMode::PROCESS_PARALLEL) { // fork
                    executeTestFuncInFork(false);
                    // Execute in fork and wait here until process ends
                } else if (getEffectiveExecutionMode() == ExecutionMode::PROCESS_SERIAL) {
                    executeTestFuncInFork(true);
                    // Execute as normal funciton
                } else if (getEffectiveExecutionMode() == ExecutionMode::FUNCTION) {
                    //                    pEpLogClass("Executing in same process and thread");
                    main_func(*this);
                    //                    pEpLogClass("Execution ended");
                } else if (getEffectiveExecutionMode() == ExecutionMode::THREAD_PARALLEL) {
                    throw runtime_error(to_string(getEffectiveExecutionMode()) + " - not implemented");
                } else if (getEffectiveExecutionMode() == ExecutionMode::THREAD_SERIAL) {
                    throw runtime_error(to_string(getEffectiveExecutionMode()) + " - not implemented");
                }
            } else {
                pEpLogClass("No function to execute");
            }
        }

        //private
        void pEpTestUnit::executeChildren() const
        {
            pEpLogClass("called");
            for (const pair<string, pEpTestUnit> elem : children) {
                elem.second.run(this);
            }
        }

        //private
        void pEpTestUnit::executeTestFuncInFork(bool wait) const
        {
            pid_t pid;
            pid = fork();
            if (pid == pid_t(0)) {
                pEpLogClass(string("pid: [" + std::to_string(getpid()) + "] - starting..."));
                setenv("HOME", getHomeDir().c_str(), 1);
                main_func(*this);
                pEpLogClass(string("pid: [" + std::to_string(getpid()) + "] - ended."));
                exit(0);
            } else if (pid < pid_t(0)) {
                pEpLogClass("Error forking");
            }
            if (wait) {
                waitChildProcesses();
            }
        }

        pEpTestModel& pEpTestUnit::getModel() const
        {
            pEpLogClass("called");
            pEpTestModel* ret = nullptr;
            if (model == nullptr) {
                ret = &parent->getModel();
            } else {
                ret = model;
            }
            assert(ret != nullptr);
            // cant be null because for createChildNode() you have to provide TestNode& instance,
            // and the only other way to get one is by createRootNode() which in turn requires a TestModel&
            return *ret;
        }

        const pEpTestUnit& pEpTestUnit::getRoot() const
        {
            pEpLogClass("called");
            const pEpTestUnit* ret = nullptr;
            if (parent != nullptr) {
                ret = &parent->getRoot();
            } else {
                ret = this;
            }
            assert(ret != nullptr);
            // cant be null because for createChildNode() you need to provide a TestNode& and
            // the only other way is using createRootNode() which has parent == nullptr
            return *ret;
        }

        string pEpTestUnit::getHomeDir() const
        {
            return getDataDir() + "/" + getName();
        }

        string pEpTestUnit::getFQName() const
        {
            string ret;
            //            pEpLogClass("called");

            if (parent != nullptr) {
                ret = parent->getFQName() + "/" + getName();
            } else {
                ret = "/" + getName();
            }
            return ret;
        }

        void pEpTestUnit::data_dir_delete()
        {
            try {
                Utils::path_delete_all(getDataDir());
            } catch (const exception& e) {
                pEpLogClass("DistTest: - could not delete data dir: " + getDataDir());
            }
        }

        void pEpTestUnit::data_dir_create()
        {
            Utils::dir_create(getDataDir());
        }

        void pEpTestUnit::data_dir_recreate()
        {
            data_dir_delete();
            data_dir_create();
        };


        void pEpTestUnit::setDefaultExecutionMode(ExecutionMode emode)
        {
            pEpTestUnit::exec_mode_default = emode;
        }

        pEpTestUnit::ExecutionMode pEpTestUnit::getExecutionMode() const
        {
            return exec_mode;
        }

        pEpTestUnit::ExecutionMode pEpTestUnit::getEffectiveExecutionMode() const
        {
            ExecutionMode ret;
            if (getExecutionMode() == ExecutionMode::INHERIT) {
                if (parent != nullptr) {
                    // get from parent until not inherit
                    ret = parent->getEffectiveExecutionMode();
                } else {
                    // get from static class config
                    ret = pEpTestUnit::exec_mode_default;
                }
            } else {
                ret = getExecutionMode();
            }
            return ret;
        }

        string pEpTestUnit::to_string(const ExecutionMode& emode)
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
    } // namespace Test
} // namespace pEp
