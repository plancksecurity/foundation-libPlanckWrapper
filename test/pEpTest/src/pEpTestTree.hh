// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HH
#define LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HH

#include <string>
#include <functional>
#include <map>
#include "../../../src/pEpLog.hh"
//#include "pEpTestModel.hh"

// Yes, the mem mgmt is purely static on purpose (so far)

namespace pEp {
    namespace Test {
        template<class T = void>
        class pEpTestTree {
        public:
            using NodeFunc = std::function<void(const pEpTestTree&)>;

            enum class ExecutionMode
            {
                FUNCTION,
                PROCESS_SEQUENTIAL,
                PROCESS_PARALLEL,
                THREAD_SEQUENTIAL, // unimplemented
                THREAD_PARALLEL,   // unimplemented
                INHERIT
            };

            // Constructors are private
            pEpTestTree() = delete;
            explicit pEpTestTree(
                pEpTestTree* parent,
                const std::string& name,
                const NodeFunc test_func = nullptr,
                T* model = nullptr,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            // Read-Only
            std::string getNodeName() const;
            std::string getNodePath() const;
            std::string getNodePathShort() const;
            T* getModel() const;
            std::string rootNodeDir() const;
            std::string processDir() const; // own process dir

            // Read-Write
            void setExecutionMode(ExecutionMode mode);
            static void setGlobalRootDir(const std::string& dir);
            static std::string getGlobalRootDir();


            // Main funcs
            void run() const;
            std::string to_string(bool recursive = true, int indent = 0) const;
            static std::string to_string(const ExecutionMode& emode);

            // logging service
            void log(const std::string& msg) const;

            // internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "pEpTestTree", debug_log_enabled };

        private:
            // CONSTRUCTORS

            // METHODS
            // Execution
            void _runRootNode() const;
            void _run() const;
            void _runSelf() const;
            void _runChildren() const;
            void _executeInFork(std::function<void(void)> func, bool wait_child) const;
            void _waitChildProcesses() const;

            // Modify
            void _addChildNode(pEpTestTree& node);

            // Query
            bool _isProcessNode() const;
            bool _isRootNode() const;
            const pEpTestTree& _rootNode() const;
            const pEpTestTree& _parentingProcessNode() const;

            // Util
            std::string _normalizeName(std::string name) const;

            // TODO
            void _data_dir_delete();
            void _data_dir_create();
            void _data_dir_recreate();

            // Fields
            const std::string _name;
            const pEpTestTree* _parent; //nullptr if RootUnit
            T* _model;                  //nullptr if inherited
            const NodeFunc _test_func;
            ExecutionMode _exec_mode;
            static std::string _global_root_dir;

            std::map<const std::string, pEpTestTree&> _children; // map to guarantee uniqueness of sibling-names

            // internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace Test
};     // namespace pEp

#include "pEpTestTree.hxx"

#endif // LIBPEPADAPTER_PEPTEST_PEPTESTTREE_HH
