// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HH
#define LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HH

#include <string>
#include <functional>
#include <map>
#include "../../../src/pEpLog.hh"
#include "pEpTestModel.hh"

// Yes, the mem mgmt is purely static on purpose (so far)

namespace pEp {
    namespace Test {
        class pEpTestTree {
        public:
            using NodeFunc = std::function<void(const pEpTestTree&)>;

            enum class ExecutionMode
            {
                FUNCTION,
                PROCESS_SERIAL,
                PROCESS_PARALLEL,
                THREAD_SERIAL,   // unimplemented
                THREAD_PARALLEL, // unimplemented
                INHERIT
            };

            // Constructors are private
            pEpTestTree() = delete;

            // Modify
            static pEpTestTree createRootNode(
                pEpTestModel& model,
                const std::string& name,
                const NodeFunc& test_func,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            static pEpTestTree createChildNode(
                pEpTestTree& parent,
                const std::string& name,
                const NodeFunc& test_func,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            // Read-Only
            std::string getNodeName() const;      // name only
            std::string getNodePath() const;      // full path (is an ID)
            std::string getNodePathShort() const; // parent process + name
            pEpTestModel& getModel() const;
            std::string getDataDir() const;

            // Read-Write
            static void setDataRoot(const std::string& dir);
            static std::string getDataRoot();

            // Main funcs
            void run(const pEpTestTree* caller = nullptr) const;
            std::string to_string(bool recursive = true, int indent = 0) const;
            static std::string to_string(const ExecutionMode& emode);

            // logging service
            void log(const std::string& msg) const;

            // internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "pEpTestTree", debug_log_enabled };

        private:
            // CONSTRUCGTORS
            explicit pEpTestTree(
                pEpTestTree* parent,
                pEpTestModel* model,
                const std::string& name,
                const NodeFunc& test_func,
                ExecutionMode exec_mode);

            // METHODS
            // Execution
            void _run() const;
            void _executeInFork(std::function<void(void)> func, bool wait_child) const;
            void _waitChildProcesses() const;

            // Modify
            void _addChildNode(pEpTestTree& node);

            // Query
            bool _isProcessNode() const;
            bool _isRootNode() const;
            const pEpTestTree& _getRootNode() const;
            const pEpTestTree& _getParentingProcessNode() const;

            // Util
            std::string _normalizeName(std::string name) const;

            // TODO
            void _data_dir_delete();
            void _data_dir_create();
            void _data_dir_recreate();

            // Fields
            const std::string _name;
            const pEpTestTree* _parent; //nullptr if RootUnit
            pEpTestModel* _model;       //nullptr if inherited
            const NodeFunc& _test_func;
            const ExecutionMode _exec_mode;
            static std::string _data_root;

            std::map<const std::string, pEpTestTree&> _children; // map to guarantee uniqueness of sibling-names

            // internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace Test
};     // namespace pEp

#endif // LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HH
