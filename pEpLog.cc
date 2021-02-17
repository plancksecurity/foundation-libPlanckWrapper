// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "pEpLog.hh"
#include <iostream> // cout and cerr
#include <sstream> // for stringstream
#include <pEp/pEpEngine.h> // for log_event()
#include "Adapter.hh" // for session()

namespace pEp {
namespace Adapter {
namespace pEpLog {

std::mutex mtx;

std::atomic_bool is_initialized{false};
std::atomic_bool is_enabled{false};
std::atomic_bool is_enabled_backend_pEpEngine{true};
std::atomic_bool is_enabled_backend_cout{false};
std::atomic_bool is_enabled_backend_cerr{false};


void set_enabled(bool enabled) {
    is_enabled.store(enabled);
}

bool get_enabled() {
    return is_enabled.load();
}

void set_enabled_cout(bool enabled){
    is_enabled_backend_cout.store(enabled);
}

bool get_enabled_cout(){
    return is_enabled_backend_cout.load();
}

void set_enabled_cerr(bool enabled){
    is_enabled_backend_cerr.store(enabled);
}

bool get_enabled_cerr(){
    return is_enabled_backend_cerr.load();
}

void set_enabled_pEpEngine(bool enabled){
    is_enabled_backend_pEpEngine.store(enabled);
}

bool get_enabled_pEpEngine(){
    return is_enabled_backend_pEpEngine.load();
}

void log_pEpEngine(std::string &msg) {
    if (!is_initialized.load()) {
        ::config_service_log(pEp::Adapter::session(), true);
        ::log_service(pEp::Adapter::session(), "pEpLog init", nullptr, nullptr, nullptr);
        is_initialized.store(true);
    }
    ::log_service(pEp::Adapter::session(), msg.c_str(), nullptr, nullptr, nullptr);
}

void log_cout(std::string &msg) {
    std::cout << msg << std::endl; //std::endl also flushes
}

void log_cerr(std::string &msg) {
    std::cerr << msg << std::endl; //std::endl also flushes
}

void log(std::string msg) {
    std::lock_guard<std::mutex> l(mtx);
    if (is_enabled.load()) {
        if (is_enabled_backend_cout.load()) {
            log_cout(msg);
        }
        if (is_enabled_backend_cerr.load()) {
            log_cerr(msg);
        }
        if (is_enabled_backend_pEpEngine.load()) {
            log_pEpEngine(msg);
        }
    }
}

} // pEpLog
} // Adapter
} // pEp
