// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_PEPLOG_HH
#define LIBPEPADAPTER_PEPLOG_HH

#include <sstream>
#include <thread>

// pEpLog
// ======
// a "to be kept small and simple" logging unit.
// featuring:
// * pEpLog macro that will be eliminated in release-builds (-DNDEBUG=1)
// * all functions thread safe (no interleave when logging from diff threads)
// * logging backend: pEpEngine logging API (providing platform abstractions and portability)
// * logging backend: cout
// * logging backend: cerr
// * runtime enabled/disabled switch (global)
// * runtime enabled/disabled switch per backend
//
// You might want more and more features, but the feature-policy is very restrictive, and there is a
// primary design goal to keep it simple, maintainable and portable.
//
// How to use:
// include <pEpLog.hh>
// use the macro pEpLog(msg) to do logging
// use NDEBUG=1 to turn logging on/off at compile-time
// use set_enabled(bool) to turn logging on/off at runtime
// use set_enabled_<backend>(bool) to turn logging on/off per backend
// All these functions are thread-safe
//
// Thats all there is to it.

#ifdef NDEBUG
    #define pEpLog(msg)  do{}while(0)
#else
    #define pEpLog(msg) \
    do {                \
        std::stringstream msg_ss; \
        msg_ss << std::this_thread::get_id() << " - " << __FILE__ << "::" << __FUNCTION__ << " - " << msg;                 \
        pEp::Adapter::pEpLog::log(msg_ss.str()); \
    } while(0)
#endif // NDEBUG

namespace pEp {
namespace Adapter {
namespace pEpLog {

void log(std::string msg);

void set_enabled(bool is_enabled);

bool get_enabled();

void set_enabled_cout(bool is_enabled);

bool get_enabled_cout();

void set_enabled_cerr(bool is_enabled);

bool get_enabled_cerr();

void set_enabled_pEpEngine(bool is_enabled);

bool get_enabled_pEpEngine();

} // pEpLog
} // Adapter
} // pEp


#endif // LIBPEPADAPTER_PEPLOG_HH

