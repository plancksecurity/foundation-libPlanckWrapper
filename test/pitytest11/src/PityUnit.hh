// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYUNIT_HH
#define PITYTEST_PITYUNIT_HH

#include "../../../src/pEpLog.hh"
#include "../../../src/std_utils.hh"
#include "fs_mutex.hh"
#include "PityTransport.hh"
#include <string>
#include <map>
#include <memory>
#include <unordered_map>

// Yes, the mem mgmt is purely static on purpose (so far)

namespace pEp {
    namespace PityTest11 {
        class PityModel;
        class PityPerspective;

        template<class T = PityModel, class P = PityPerspective>
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
            explicit PityUnit<T, P>(
                PityUnit<>* parent,
                const std::string& name,
                const std::function<void(PityUnit&, T*, P*)> test_func = nullptr,
                T* model = nullptr,
                P* perspective = nullptr,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            // Read-Only
            std::string getName() const;
            std::string getPath() const;
            std::string getPathShort() const;
            std::string processDir(); // own process dir
            std::string transportDir();
            // Read-Write
            //            void setExecutionMode(ExecutionMode mode);
            static void setGlobalRootDir(const std::string& dir);
            static std::string getGlobalRootDir();


            // Main funcs
            void run();
            std::string to_string(bool recursive = true, int indent = 0);
            static std::string to_string(const ExecutionMode& emode);

            // Util
            void recreateDirsRecursively();

            //Transport
            PityTransport* transport() const;
            void registerAsTransportEndpoint();
            Endpoints& transportEndpoints();


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
            void _init();
            void _run();
            void _runSelf();
            void _runChildren() const;
            void _executeInFork(std::function<void(void)> func, bool wait_child) const;
            void _waitChildProcesses() const;

            // Modify
            void _addChildUnit(PityUnit& unit);

            // Query
            bool _isProcessUnit() const;
            bool _isRootUnit() const;
            PityUnit* _rootUnit();
            std::string _rootUnitDir();
            const PityUnit& _parentingProcessUnit() const;
            T* _getModel() const;
            P* _getPerspective() const;
            void createTransport();


            // Util
            std::string _normalizeName(std::string name) const;
            std::string _status_string(const std::string& msg) const;
            Utils::Color _colForProcUnitNr(int procUnitNr) const;
            Utils::Color _termColor() const;
            void logRaw(const std::string& msg) const;

            // Fields
            const std::string _name;
            PityUnit* _parent; //nullptr if RootUnit
            T* _model;               //nullptr if inherited
            P* _perspective;         //nullptr if inherited
            const std::function<void(PityUnit&, T*, P*)> _test_func;
            ExecutionMode _exec_mode;
            static std::string _global_root_dir;
            std::map<const std::string, PityUnit&> _children; // map to guarantee uniqueness of sibling-names
            int procUnitNr;
            static int procUnitsCount;                 // will be increased in everuy constructor
            std::shared_ptr<PityTransport> _transport; //only ever read via transport()
            Endpoints _transport_endpoints;            // only ever access via transportEndpoints()

            std::shared_ptr<fs_mutex> _log_mutex = nullptr;
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
