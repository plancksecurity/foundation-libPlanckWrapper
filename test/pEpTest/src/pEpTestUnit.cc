#include "pEpTestUnit.hh"
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

namespace pEp {
    namespace Test {
        bool pEpTestUnit::log_enabled = true;
        string pEpTestUnit::data_root = "./peptest";
        pEpTestUnit::ExecutionMode pEpTestUnit::emode_default = pEpTestUnit::ExecutionMode::FUNCTION;

        // RootNode factory
        // static
        pEpTestUnit pEpTestUnit::createRootNode(
            pEpTestModel& model,
            const std::string& name,
            const TestUnitFunction& test_func,
            ExecutionMode emode_children)
        {
            pEpTestUnit ret(nullptr, &model, name, test_func, emode_children);
            return ret;
        }

        // ChildNode factory
        // static
        pEpTestUnit pEpTestUnit::createChildNode(
            pEpTestUnit& parent,
            const std::string& name,
            const TestUnitFunction& test_func,
            ExecutionMode emode_children)
        {
            pEpTestUnit ret(&parent, nullptr, name, test_func, emode_children);
            return ret;
        }


        // private
        pEpTestUnit::pEpTestUnit(
            pEpTestUnit* const parent,
            pEpTestModel* model,
            const std::string& name,
            const TestUnitFunction& test_func,
            ExecutionMode emode_children) :
            parent(parent),
            model(model), name(normalizeName(name)), test_func(test_func), emode_chld(emode_children)
        {
            logger.set_instancename(getFQName());
            if (!isRootNode()) {
                parent->addChildNode(*this);
            }
        }

        // static
        void pEpTestUnit::setDefaultDataRoot(const std::string& dir)
        {
            pEpTestUnit::data_root = dir;
        }

        // static
        std::string pEpTestUnit::getDataRoot()
        {
            return pEpTestUnit::data_root;
        }

        void pEpTestUnit::init(const pEpTestUnit* caller) const
        {
            //caller is never nullptr if called from another unit
            if (caller == nullptr) {}

            //            for (const pair<string, pEpTestUnit&> elem : testnodes) {
            //                string home_dir = evalHomeDir(elem.second);
            //                pEpLogClass("creating home dir for '" + elem.second.getName() + "' - " + home_dir);             mkdir(home_dir.c_str(), 0770);
            //            }
        }

        void pEpTestUnit::addChildNode(pEpTestUnit& node)
        {
            children.insert(pair<string, pEpTestUnit&>(node.getName(), node));
        }

        // name is alphanumeric only (everything else will be replaced by an underscore)
        // private
        string pEpTestUnit::normalizeName(string name) const
        {
            replace_if(
                name.begin(),
                name.end(),
                [](char c) -> bool { return !isalnum(c); },
                '_');

            return name;
        }

        string pEpTestUnit::getName() const
        {
            return name;
        }

        // RootNodes have their own data_dir
        // ProcessNodes have their own data_dir inside their RootNote dir (nameclash prossible)
        // All other nodes inherit data_dir from their Root/ProcessNode
        string pEpTestUnit::getDataDir() const
        {
            string ret;
            if (isRootNode()) {
                ret = getDataRoot() + getName() + "/";
            } else {
                if (parent->getEffectiveExecutionMode() == ExecutionMode::PROCESS_SERIAL ||
                    parent->getEffectiveExecutionMode() == ExecutionMode::PROCESS_PARALLEL) {

                    ret = parent->getDataDir() + getName();
                } else {
                    // inherit
                    ret = parent->getDataDir();
                }
            }
            return ret;
        }

        void pEpTestUnit::run(const pEpTestUnit* caller)
        {
            pEpLogClass("called");
            // caller is never nullptr if called by another pEpTestUnit
            if (caller == nullptr) {
                pEpLogClass("\n" + to_string());
            }

            if (test_func) {
                test_func(*this);
            } else {
                pEpLogClass("No function to execute");
            }
            executeChildren();
        }

        // private
        void pEpTestUnit::executeChildren() const
        {
            if (!children.empty()) {
                const ExecutionMode& emode = getEffectiveExecutionMode();
                for (const pair<string, pEpTestUnit&> elem : children) {
                    // Execute in fork and wait here until process ends
                    if (emode == ExecutionMode::PROCESS_SERIAL) { // fork
                        executeForked(elem.second);
                        waitChildProcesses();
                        // Execute in fork and go on, wait for process execution in the end
                    } else if (emode == ExecutionMode::PROCESS_PARALLEL) {
                        executeForked(elem.second);
                        // Execute as normal funciton
                    } else if (emode == ExecutionMode::FUNCTION) {
                        elem.second.run(this);
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

        // private
        void pEpTestUnit::executeForked(pEpTestUnit& unit) const
        {
            pid_t pid;
            pid = fork();
            if (pid == pid_t(0)) {
                //                setenv("HOME", evalHomeDir().c_str(), 1);
                unit.run(this);
                exit(0);
            } else if (pid < pid_t(0)) {
                pEpLogClass("Error forking");
            }
        }

        // private
        void pEpTestUnit::waitChildProcesses() const
        {
            int status;
            pid_t pid;
            while ((pid = wait(&status)) > 0) {
                pEpLogClass(
                    "process[" + to_string((int)pid) +
                    "] terminated with status: " + to_string(status));
            }
        }

        // Inherited (if null see parent recursively)
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
            if (!isRootNode()) {
                ret = &parent->getRoot();
            } else {
                ret = this;
            }
            assert(ret != nullptr);
            // cant be null because for createChildNode() you need to provide a TestNode& and
            // the only other way is using createRootNode() which has parent == nullptr
            return *ret;
        }

        string pEpTestUnit::getFQNameNormalized() const
        {
            return normalizeName(getFQName());
        }

        string pEpTestUnit::getFQName() const
        {
            //            pEpLogClass("called");
            string ret;

            if (!isRootNode()) {
                ret = parent->getFQName() + "/" + getName();
            } else {
                ret = getName();
            }
            return ret;
        }

        //        // A process node is a node that spawns its own process
        //        bool pEpTestUnit::isProcessNode() const
        //        {
        //            bool ret = false;
        //            if (isRootNode() ||
        //                parent->getEffectiveExecutionMode() == ExecutionMode::PROCESS_PARALLEL ||
        //                parent->getEffectiveExecutionMode() == ExecutionMode::PROCESS_SERIAL) {
        //                ret = true;
        //            }
        //            return ret;
        //        }

        bool pEpTestUnit::isRootNode() const
        {
            bool ret = false;
            if (parent == nullptr) {
                ret = true;
            }
            return ret;
        }

        void pEpTestUnit::data_dir_delete()
        {
            try {
                Utils::path_delete_all(getDataRoot());
            } catch (const exception& e) {
                pEpLogClass("DistTest: - could not delete data dir: " + getDataRoot());
            }
        }

        void pEpTestUnit::data_dir_create()
        {
            //            Utils::dir_create(getDataDir());
            pEpLogClass("creating dir:" + getDataRoot());
        }

        void pEpTestUnit::data_dir_recreate()
        {
            data_dir_delete();
            data_dir_create();
        };

        // static
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
                if (!isRootNode()) {
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

        string pEpTestUnit::to_string(bool recursive, int indent) const
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

        const pEpTestUnit& pEpTestUnit::getParentProcessUnit() const
        {
            if (!isRootNode()) {
                if (parent->getEffectiveExecutionMode() == ExecutionMode::PROCESS_PARALLEL ||
                    parent->getEffectiveExecutionMode() == ExecutionMode::PROCESS_SERIAL ||
                    parent->isRootNode()) {
                    return *this;
                } else {
                    return parent->getParentProcessUnit();
                }
            } else {
                return *this;
            }
        }

        //Well, ok, lets just add some little convenience logging service in here, too
        void pEpTestUnit::log(const string& msg) const
        {
            stringstream builder;
            builder << "[" << std::to_string(getpid()) << "/" << getParentProcessUnit().getFQName()
                    << "] -  ";
            builder << msg << endl;
            cout << builder.str();
        }
    } // namespace Test
} // namespace pEp
