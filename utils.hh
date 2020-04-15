#pragma once


#ifdef NDEBUG
    #define pEpLog(msg)  do{}while(0)
#else
    #include <iostream>
    #define pEpLog(msg) do{std::cerr << __FILE__ << "::" << __FUNCTION__ << " - " << msg << '\n';} while(0)
#endif
