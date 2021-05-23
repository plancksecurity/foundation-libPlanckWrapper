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
        pEpTestUnit::ExecutionMode pEpTestUnit::emode_default = pEpTestUnit::ExecutionMode::FUNCTION;

        // RootNode factory
        //static
        pEpTestUnit pEpTestUnit::createRootNode(
            pEpTestModel& model,
            const std::string& name,
            const TestUnitFunction& main_f,
            ExecutionMode emode_children)
        {
            pEpTestUnit ret(nullptr, &model, name, main_f, emode_children);
            return ret;
        }

        // ChildNode factory
        //static
        pEpTestUnit pEpTestUnit::createChildNode(
            pEpTestUnit& parent,
            const std::string& name,
            const TestUnitFunction& main_f,
            ExecutionMode emode_children)
        {
            pEpTestUnit ret(&parent, nullptr, name, main_f, emode_children);
            return ret;
        }

        //private
        pEpTestUnit::pEpTestUnit(
            pEpTestUnit* const parent,
            pEpTestModel* model,
            const std::string& name,
            const TestUnitFunction& main_f,
            ExecutionMode emode_children) :
            parent(parent),
            model(model), name(name), main_func(main_f), emode_chld(emode_children)
        {
            logger.set_instancename(getFQName());
            if (parent != nullptr) {
                parent->addChildNode(*this);
            }
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
            pEpLogClass("Waiting for child processes to terminate...");
            int status;
            pid_t pid;
            while ((pid = wait(&status)) > 0) {
                pEpLogClass(
                    "process[" + std::to_string((int)pid) +
                    "] terminated with status: " + std::to_string(status));
            }
            pEpLogClass("All child processes terminated");
        }

        void pEpTestUnit::addChildNode(const pEpTestUnit& node)
        {
            children.insert(pair<string, const pEpTestUnit&>(node.getName(), node));
        }

        string pEpTestUnit::getName() const
        {
            return name;
        }

        void pEpTestUnit::run(const pEpTestUnit* caller) const
        {
            pEpLogClass("called");
            // caller is never nullptr if called by another pEpTestUnit
            if(caller == nullptr) {
                pEpLogClass("\n" + to_string());
            }

            if (main_func) {
                main_func(*this);
            } else {
                pEpLogClass("No function to execute");
            }
            executeChildren();
        }

        //private
        void pEpTestUnit::executeChildren() const
        {
            if (!children.empty()) {
                const ExecutionMode& emode = getEffectiveExecutionMode();
                pEpLogClass(string("Executing children as: " + to_string(emode)));
                for (const pair<string, const pEpTestUnit&> elem : children) {
                    // Execute in fork and go on, wait for process execution in the end
                    if (emode == ExecutionMode::PROCESS_PARALLEL) { // fork
                        executeForked(elem.second, false);
                        // Execute in fork and wait here until process ends
                    } else if (emode == ExecutionMode::PROCESS_SERIAL) {
                        executeForked(elem.second, true);
                        // Execute as normal funciton
                    } else if (emode == ExecutionMode::FUNCTION) {
                        pEpLogClass("Executing in same process and thread");
                        elem.second.run(this);
                        pEpLogClass("Execution ended");
                    } else if (emode == ExecutionMode::THREAD_PARALLEL) {
                        throw runtime_error(to_string(emode) + " - not implemented");
                    } else if (emode == ExecutionMode::THREAD_SERIAL) {
                        throw runtime_error(to_string(emode) + " - not implemented");
                    }
                }
                if (emode == ExecutionMode::PROCESS_PARALLEL) {
                    waitChildProcesses();
                }
            }
        }

        //private
        void pEpTestUnit::executeForked(const pEpTestUnit& unit, bool wait) const
        {
            pid_t pid;
            pid = fork();
            if (pid == pid_t(0)) {
                pEpLogClass(string("In pid: [" + std::to_string(getpid()) + "] - starting..."));
                setenv("HOME", getHomeDir().c_str(), 1);
                unit.run(this);
                pEpLogClass(string("In pid: [" + std::to_string(getpid()) + "] - ended."));
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
            //            pEpLogClass("called");
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
            //            pEpLogClass("called");
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

        //static
        void pEpTestUnit::setDefaultExecutionMode(ExecutionMode emode)
        {
            pEpTestUnit::emode_default = emode;
        }

        pEpTestUnit::ExecutionMode pEpTestUnit::getExecutionMode() const
        {
            return emode_chld;
        }

        pEpTestUnit::ExecutionMode pEpTestUnit::getEffectiveExecutionMode() const
        {
            ExecutionMode ret{ emode_default };
            if (getExecutionMode() == ExecutionMode::INHERIT) {
                if (parent != nullptr) {
                    // get from parent until not inherit
                    ret = parent->getEffectiveExecutionMode();
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

        std::string pEpTestUnit::to_string(bool recursive, int indent) const
        {
            string ret;
            stringstream builder;
            builder << string(indent, '\t') << getName();
            builder << "[children=" << to_string(getEffectiveExecutionMode()) + "]";
            //            builder << "/" << getModel().getName();
            builder << endl;
            ret = builder.str();

            if (recursive) {
                indent++;
                for (const pair<string, const pEpTestUnit&> elem : children) {
                    ret += elem.second.to_string(true, indent);
                }
                indent--;
            }
            return ret;
        }
    } // namespace Test
} // namespace pEp
