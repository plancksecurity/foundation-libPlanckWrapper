#pragma once
#include <iostream>
#include <thread>
#include <sstream>
#include <iomanip>

// Returns a timestamp string featuring microseconds precision
// Uses system clock, so time might not pass uniformly

inline std::string timestamp_usec() {
    std::ostringstream buffer;
    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();

    std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(d);
    std::chrono::seconds       s = std::chrono::duration_cast<std::chrono::seconds>     (d);

    std::time_t t = s.count();
    std::tm tm = *std::localtime(&t);

    std::size_t fractional_seconds = us.count() % 1000000;
    buffer << std::put_time(&tm, "%H%:%M:%S") << "." << fractional_seconds;

    return buffer.str();
}

// pEpLog(msg) - logs to STDOUT
// pEpErr(msg) - logs to STDERR
//
// Log format is:
// TIMESTAMP.usecs THREAD-ID SRC_FILE_NAME::FUNCTION_NAME - msg
//
// example:
// 21:06:04.214884 Thread:0x10f2ce5c0 test_peplog.cc::main - test run nr: 436744
//
// if -DNDEBUG=1 (for release builds), all pEpLog() calls will be optimized away.

#ifdef NDEBUG
#define pEpLog(msg) do{}while(0)
#else
#define pEpLog(msg) do{ std::cout << std::unitbuf; std::cout << timestamp_usec() << " Thread:" << std::this_thread::get_id() << ' ' <<__FILE__ << "::" << __FUNCTION__ << " - " << msg << std::endl;} while(0)
#endif
#define pEpErr(msg) do{ std::cerr << std::unitbuf; std::cerr << timestamp_usec() << " Thread:" << std::this_thread::get_id() << ' ' <<__FILE__ << "::" << __FUNCTION__ << " - " << msg << std::endl;} while(0)
