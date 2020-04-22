// TODO: put into not yet existing libpEpAdapter_utils.h, to be across whole libpEpAdapter
#ifdef NDEBUG
    #define pEpLog(msg)  do{}while(0)
#else
    #include <iostream>
    #define pEpLog(msg) do{std::cerr << __FILE__ << "::" << __FUNCTION__ << " - " << msg << '\n';} while(0)
#endif