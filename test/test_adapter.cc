// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework.hh"
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <sys/param.h>

#include <pEpLog.hh>
#include <Adapter.hh>
#include <pEp/sync_api.h>
#include <pEp/keymanagement.h>
#include <pEp/message_api.h>

using namespace std;
using namespace pEp;

PEP_STATUS messageToSend(struct _message *msg)
{
    pEpLog("called()");
    return PEP_STATUS_OK;
}

PEP_STATUS notifyHandshake(::pEp_identity *me, ::pEp_identity *partner, ::sync_handshake_signal signal)
{
    pEpLog("called()");
    return PEP_STATUS_OK;
}

int main(int argc, char **argv)
{
    Test::setup(argc, argv);

    // Create new identity
    pEpLog("updating or creating identity for me");
    ::pEp_identity *me = new_identity("alice@peptest.ch", NULL, "23", "Who the F* is Alice");
    assert(me);
    ::PEP_STATUS status = ::myself(Adapter::session(), me);
    ::free_identity(me);
    pEp::throw_status(status);

    // start and stop sync repeatedly
    useconds_t sleepuSec = 1000 * 100;
    unsigned long long int nrIters = 1000 * 1000 * 1000;
    for (int i = 0; i < nrIters; i++) {
        pEpLog("RUN NR: ");
        pEpLog(i);
        pEpLog("SYNC START");
        pEpLog("starting the adapter including sync");
        Adapter::startup(messageToSend, notifyHandshake);
        pEpLog("SYNC STOP");
        usleep(sleepuSec);
        Adapter::shutdown();
    }
    return 0;
}
