// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "utils.hh"
// Single threaded test of pEpLog() function.
// check for uniformity of time

int main()
{
    pEpLog("Test Starting... observe uniformity of timestamp.");
    int nr_log_lines = 1000000;
    for (int i; i < nr_log_lines; i++) {
        pEpLog("log line nr: " << i);
    }
    return 0;
}
