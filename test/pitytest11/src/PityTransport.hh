// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYTRANSPORT_HH
#define PITYTEST_PITYTRANSPORT_HH

#include "../../../src/pEpLog.hh"
#include <vector>
#include <memory>
#include <unordered_map>

namespace pEp {
    namespace PityTest11 {
        // Address - Dir
         using Endpoints = std::unordered_map<std::string, std::string>;

        class PityTransport {
        public:
            // Constructors
            PityTransport() = delete;
            PityTransport(const std::string& inboxDir, Endpoints& endpoints);

            // Getters
            //Transport
            bool hasMsg() const;
            void sendMsg(const std::string nodename, const std::string& msg) const;
            std::string pollMsg() const;
            std::string receiveMsg(int timeout_msec = 100) const;

            //internal logging
            static bool debug_log_enabled;
            Adapter::pEpLog::pEpLogger logger_debug{ "PityModel", debug_log_enabled };

        private:
            std::string _inboxDir;
            Endpoints& _endpoints;

            //internal logging
            Adapter::pEpLog::pEpLogger& m4gic_logger_n4me = logger_debug;
        };
    }; // namespace PityTest11

}; // namespace pEp

#endif // PITYTEST_PITYTRANSPORT_HH
