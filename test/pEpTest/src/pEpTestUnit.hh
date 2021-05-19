// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PEPTESTUNIT_HH
#define LIBPEPADAPTER_PEPTESTUNIT_HH

#include <string>
#include <functional>
#include <map>
#include "../../../src/pEpLog.hh"
#include "pEpTestModel.hh"

namespace pEp {
    namespace Test {
        class pEpTestUnit {
        public:
            using TestEntryFunc = std::function<void(const pEpTestUnit&)>;
            enum class ExecutionMode
            {
                FUNCTION,
                PROCESS_SERIAL,
                PROCESS_PARALLEL,
                THREAD_SERIAL,   // unimplemented
                THREAD_PARALLEL, // unimplemented
                INHERIT
            };
            static std::string to_string(const ExecutionMode& emode);

            // Constructors
            pEpTestUnit() = delete;

            // Methods
            static pEpTestUnit createRootNode(
                pEpTestModel& model,
                const std::string& name,
                const TestEntryFunc& main_f,
                ExecutionMode emode = ExecutionMode::INHERIT);

            static pEpTestUnit createChildNode(
                pEpTestUnit& parent,
                const std::string& name,
                const TestEntryFunc& main_f,
                ExecutionMode emode = ExecutionMode::INHERIT);


            static void setDefaultDataDir(const std::string& dir);
            static std::string getDataDir();
            void addChildNode(const pEpTestUnit& node);
            void run(const pEpTestUnit* caller = nullptr) const;
            std::string getName() const;
            std::string getFQName() const;
            pEpTestModel& getModel() const;
            std::string getHomeDir() const;
            static void setDefaultExecutionMode(ExecutionMode emode);
            ExecutionMode getExecutionMode() const;
            ExecutionMode getEffectiveExecutionMode() const;

        private:
            // Constructors
            explicit pEpTestUnit(
                pEpTestUnit* parent,
                pEpTestModel* model,
                const std::string& name,
                const TestEntryFunc& main_f,
                ExecutionMode emode);

            // Methods
            void init() const;
            const pEpTestUnit& getRoot() const;
            void executeSelf() const;
            void executeChildren() const;

            void executeTestFuncInFork(bool wait) const;
            void waitChildProcesses() const;
            void data_dir_delete();
            void data_dir_create();
            void data_dir_recreate();

            // Fields
            const pEpTestUnit* parent; //nullptr if rootnode
            const std::string name;
            pEpTestModel* model; //nullptr if inherited
            const TestEntryFunc& main_func;
            ExecutionMode exec_mode;
            static ExecutionMode exec_mode_default;
            std::map<const std::string, pEpTestUnit> children;
            static std::string data_dir;

            // logger
            static bool log_enabled;
            Adapter::pEpLog::pEpLogger logger{ "pEpTestUnit", log_enabled };
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger;
        };
    }; // namespace Test
};     // namespace pEp

#endif // LIBPEPADAPTER_PEPTESTUNIT_HH
