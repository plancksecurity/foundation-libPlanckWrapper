// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYUNIT_HH
#define PITYTEST_PITYUNIT_HH

#include <string>
#include <map>
#include "../../../src/pEpLog.hh"
#include "../../../src/std_utils.hh"

// Yes, the mem mgmt is purely static on purpose (so far)

namespace pEp {
    namespace PityTest11 {
        template<class T = void>
        class PityUnit {
        public:
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
            PityUnit() = delete;
            explicit PityUnit<T>(
                PityUnit<T>* parent,
                const std::string& name,
                const std::function<void(const PityUnit&)> test_func = nullptr,
                T* model = nullptr,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            // Read-Only
            std::string getName() const;
            std::string getPath() const;
            std::string getPathShort() const;
            T* getModel() const;
            std::string processDir() const; // own process dir

            // Read-Write
            //            void setExecutionMode(ExecutionMode mode);
            static void setGlobalRootDir(const std::string& dir);
            static std::string getGlobalRootDir();


            // Main funcs
            void run() const;
            std::string to_string(bool recursive = true, int indent = 0) const;
            static std::string to_string(const ExecutionMode& emode);

            // Util
            void recreateDirsRecursively() const;

            // logging service
            void log(const std::string& msg) const;
            void logH1(const std::string& msg) const;
            void logH2(const std::string& msg) const;
            void logH3(const std::string& msg) const;

            // internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityUnit", debug_log_enabled };

        private:
            // CONSTRUCTORS

            // METHODS
            // Execution
            void _init() const;
            void _run() const;
            void _runSelf() const;
            void _runChildren() const;
            void _executeInFork(std::function<void(void)> func, bool wait_child) const;
            void _waitChildProcesses() const;

            // Modify
            void _addChildUnit(PityUnit& unit);

            // Query
            bool _isProcessUnit() const;
            bool _isRootUnit() const;
            const PityUnit& _rootUnit() const;
            std::string _rootUnitDir() const;
            const PityUnit& _parentingProcessUnit() const;

            // Util
            std::string _normalizeName(std::string name) const;
            std::string _status_string(const std::string& msg) const;
            Utils::Color _colForProcUnitNr(int procUnitNr) const;
            Utils::Color _termColor() const;
            void logRaw(const std::string& msg) const;

            // Dirs
            void _ensureDir(const std::string& path) const;
            void _recreateDir(const std::string& path) const;


            // Fields
            const std::string _name;
            const PityUnit* _parent; //nullptr if RootUnit
            T* _model;               //nullptr if inherited
            const std::function<void(const PityUnit&)> _test_func;
            ExecutionMode _exec_mode;
            static std::string _global_root_dir;
            std::map<const std::string, PityUnit&> _children; // map to guarantee uniqueness of sibling-names
            int procUnitNr;
            static int procUnitsCount; // will be increased in everuy constructor

            // internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };


        class PityAssertException : public std::runtime_error {
        public:
            PityAssertException(const std::string& string) : runtime_error(string) {}
        };


#ifndef PTASSERT
    #define PTASSERT(condition)                                                                    \
        do {                                                                                       \
            if (!(condition)) {                                                                    \
                throw PityAssertException("AssertError");                                          \
            }                                                                                      \
        } while (0)
#endif

#ifndef PTASSERT_EXCEPT
    #define PTASSERT_EXCEPT(func)                                                                  \
        do {                                                                                       \
            try {                                                                                  \
                (func);                                                                            \
                PTASSERT(false);                                                                   \
            } catch (const exception& e) {                                                         \
            }                                                                                      \
        } while (0)
#endif

    }; // namespace PityTest11
};     // namespace pEp

#include "PityUnit.hxx"

#endif // PITYTEST_PITYUNIT_HH
