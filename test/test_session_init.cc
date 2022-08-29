// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <sys/param.h>

#include <pEpLog.hh>
#include <Adapter.hh>

using namespace std;
using namespace pEp;

int main(int argc, char **argv)
{
//    ::PEP_SESSION session_;
//    ::PEP_STATUS status = ::init(&session_, NULL,  NULL, NULL);

    Adapter::session();
    return 0;
}
