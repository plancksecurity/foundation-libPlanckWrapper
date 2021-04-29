#include "utils.hh"


void sleep_millis(int milis) {
    std::chrono::milliseconds timespan(milis);
    std::this_thread::sleep_for(timespan);
}
