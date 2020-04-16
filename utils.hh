#pragma once

#include <iostream>
#include <thread>

#ifdef NDEBUG
    #define pEpLog(msg)  do{}while(0)
#else
    #define pEpLog(msg) do{std::cout << "Thread:" << std::this_thread::get_id() << ' ' <<__FILE__ << "::" << __FUNCTION__ << " - " << msg << '\n';} while(0)
#endif

#define pEpErr(msg) do{std::cerr << "Thread:" << std::this_thread::get_id() << ' ' <<__FILE__ << "::" << __FUNCTION__ << " - " << msg << '\n';} while(0)