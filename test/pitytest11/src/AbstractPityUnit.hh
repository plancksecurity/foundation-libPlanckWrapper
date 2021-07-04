// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_ABSTRACTPITYUNIT_HH
#define PITYTEST_ABSTRACTPITYUNIT_HH

#include "../../../src/pEpLog.hh"
#include "../../../src/std_utils.hh"
#include "fs_mutex.hh"
#include "PityTransport.hh"
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <functional>

// Yes, the mem mgmt is purely static on purpose (so far)

namespace pEp {
    namespace PityTest11 {
        class AbstractPityUnit {
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

            AbstractPityUnit(
                AbstractPityUnit* const parent,
                const std::string& name,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            // Read-Write
            static void setGlobalRootDir(const std::string& dir);
            static std::string getGlobalRootDir();

            // Read-Only
            std::string getName() const;
            std::string getPath() const;
            std::string getPathShort() const;
            std::string processDir(); // own process dir
            std::string transportDir();
            AbstractPityUnit* getParent() const;
            bool _isRootUnit() const; // true if has no parent

            // Main funcs
            void run();

            std::string to_string(bool recursive = true, int indent = 0);
            static std::string to_string(const ExecutionMode& emode);

            // logging service
            void log(const std::string& msg) const;
            void logH1(const std::string& msg) const;
            void logH2(const std::string& msg) const;
            void logH3(const std::string& msg) const;

            // Util
            static std::string _normalizeName(
                std::string name); //TODO HACK in PityTransport this should be private
            void recreateDirsRecursively();

            //Transport
            PityTransport* transport() const;
            void registerAsTransportEndpoint();
            Endpoints& transportEndpoints();

            // internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityUnit", debug_log_enabled };

        protected:
            std::string _status_string(const std::string& msg) const;
            static Utils::Color _colForProcUnitNr(int procUnitNr);
            Utils::Color _termColor() const;
            void _logRaw(const std::string& msg) const;

            // internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;

        private:
            // METHODS
            // Execution
            void _init();
            void _run();
            virtual void _runSelf() = 0;
            void _runChildren() const;
            void _executeInFork(std::function<void(void)> func, bool wait_child) const;
            void _waitChildProcesses() const;


            // Modify
            void _addChildUnit(AbstractPityUnit& unit);

            AbstractPityUnit* rootUnit();
            const AbstractPityUnit& parentingProcessUnit() const;

            // Query
            bool _isProcessUnit() const; // true if it forks
            std::string _rootUnitDir();

            // Transport
            void _createTransport();


            // Fields
            // ------
            AbstractPityUnit* _parent;                                //nullptr if RootUnit
            std::map<const std::string, AbstractPityUnit&> _children; // map to guarantee uniqueness of sibling-names
            static std::string _global_root_dir;
            const std::string _name;
            int procUnitNr;
            ExecutionMode _exec_mode;
            static int procUnitsCount; // will be increased in every constructor
            // transport
            std::shared_ptr<PityTransport> _transport; //only ever read via transport()
            Endpoints _transport_endpoints;            // only ever access via transportEndpoints()

            // fs-mutex to sync across processes
            std::shared_ptr<fs_mutex> _log_mutex = nullptr;
        };

        class PityAssertException : public std::runtime_error {
        public:
            PityAssertException(const std::string& string) : runtime_error(string) {}
        };

#ifndef PTASSERT
    #define PTASSERT(condition, msg)                                                               \
        do {                                                                                       \
            if (!(condition)) {                                                                    \
                throw PityAssertException(msg);                                                    \
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

#endif
