// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_ABSTRACTPITYUNIT_HH
#define PITYTEST_ABSTRACTPITYUNIT_HH

#include "../../../src/pEpLog.hh"
#include "../../../src/std_utils.hh"
#include "fs_mutex.hh"
#include "PityTree.hh"
#include "PityTransport.hh"
#include <string>
#include <memory>
#include <functional>

// Yes, the mem mgmt is purely static on purpose (so far)

namespace pEp {
    namespace PityTest11 {
        class AbstractPityUnit : public PityTree<AbstractPityUnit> {
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

            // Constructors
            // ------------
            // RootNode
            explicit AbstractPityUnit(
                const std::string& name,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            // LeafNode
            explicit AbstractPityUnit(
                AbstractPityUnit& parent,
                const std::string& name,
                ExecutionMode exec_mode = ExecutionMode::FUNCTION);

            // Copy
            explicit AbstractPityUnit(const AbstractPityUnit& rhs, AbstractPityUnit& self);

            // copy-assign
            AbstractPityUnit& operator=(const AbstractPityUnit& rhs);

            AbstractPityUnit* clone() override = 0;

            // Read-Write
            static void setGlobalRootDir(const std::string& dir);
            static std::string getGlobalRootDir();
            void setExecMode(ExecutionMode execMode);

            // Read-Only
            std::string getPathShort() const;
            std::string getProcessDir(); // own process dir
            std::string getTransportDir();
            std::string getRootUnitDir();
            bool isProcessUnit() const; // true if it forks
            const AbstractPityUnit& getParentProcessUnit() const;


            // Main funcs
            void run(bool init_tree = true);

            std::string to_string(bool recursive = true, int indent = 0);
            static std::string to_string(const ExecutionMode& emode);

            // logging service
            void log(const std::string& msg) const;
            void logH1(const std::string& msg) const;
            void logH2(const std::string& msg) const;
            void logH3(const std::string& msg) const;

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
            Utils::Color _color() const;
            void _logRaw(const std::string& msg) const;

            // internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;

        private:
            // METHODS
            void _init();
            // Execution
            void _initProcUnitNrRecurse();
            void _initTransportRecurse();
            void _initDirsRecursive();
            void _runRecurse();
            virtual void _runSelf() = 0;
            void _executeInFork(std::function<void(void)> func, bool wait_child) const;
            void _waitChildProcesses() const;

            // Transport
            void _createTransport();

            // Fields
            // ------
            static std::string _global_root_dir;
            ExecutionMode _exec_mode;
            int _procUnitNr;
            static int _procUnitsCount; // will be increased in every constructor

            // transport
            std::shared_ptr<PityTransport> _transport; //only ever read via transport()
            // TODO move endpoints into PityTransport
            Endpoints _transport_endpoints; // only ever access via transportEndpoints()

            // fs-mutex to sync across processes
            std::shared_ptr<fs_mutex> _log_mutex = nullptr;
        };

        class PityAssertException : public std::runtime_error {
        public:
            PityAssertException(const std::string& string) : runtime_error(string) {}
        };
    }; // namespace PityTest11
};     // namespace pEp

#endif
