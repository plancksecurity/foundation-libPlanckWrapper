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
            using TestUnitFunction = std::function<void(const pEpTestUnit&)>;

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
                const TestUnitFunction& test_func,
                ExecutionMode emode_children = emode_default);

            static pEpTestUnit createChildNode(
                pEpTestUnit& parent,
                const std::string& name,
                const TestUnitFunction& test_func,
                ExecutionMode emode_children = emode_default);

            void init(const pEpTestUnit* caller = nullptr) const;
            static void setDefaultDataRoot(const std::string& dir);
            static std::string getDataRoot();
            void addChildNode(pEpTestUnit& node);
            void run(const pEpTestUnit* caller = nullptr);
            std::string getName() const;
            std::string getFQName() const;
            std::string getFQNameNormalized() const;
            pEpTestModel& getModel() const;
            std::string getDataDir() const;
            const pEpTestUnit& getParentProcessUnit() const;
            std::string to_string(bool recursive = true, int indent = 0) const;
            static void setDefaultExecutionMode(ExecutionMode emode);

            //logging service
            void log(const std::string& msg) const;

            // internal logging
            static bool log_enabled;
            Adapter::pEpLog::pEpLogger logger{ "pEpTestUnit", log_enabled };

        private:
            // Constructors
            explicit pEpTestUnit(
                pEpTestUnit* parent,
                pEpTestModel* model,
                const std::string& name,
                const TestUnitFunction& test_func,
                ExecutionMode emode_children);

            // Methods
            const pEpTestUnit& getRoot() const;
            void executeChildren() const;
            void executeForked(pEpTestUnit& unit) const;
            void waitChildProcesses() const;
            ExecutionMode getExecutionMode() const;
            ExecutionMode getEffectiveExecutionMode() const;
            void data_dir_delete();
            void data_dir_create();
            void data_dir_recreate();
            bool isRootNode() const;
            //            bool isProcessNode() const;
            std::string normalizeName(std::string name) const;

            // Fields
            const pEpTestUnit* parent; //nullptr if rootnode
            const std::string name;
            pEpTestModel* model; //nullptr if inherited
            const TestUnitFunction& test_func;
            const ExecutionMode emode_chld;
            static ExecutionMode emode_default;
            std::map<const std::string, pEpTestUnit&> children;
            static std::string data_root;

            // logger
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger;
        };
    }; // namespace Test
};     // namespace pEp

#endif // LIBPEPADAPTER_PEPTESTUNIT_HH
