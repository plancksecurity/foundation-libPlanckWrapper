// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"
#include <pEp/pitytest11/test_utils.hh>
#include <iostream>
#include <cassert>

#include <pEp/sync_api.h>
#include <pEp/keymanagement.h>
#include "../src/Adapter.hh"
#include "../src/utils.hh"

using namespace std;
using namespace pEp;

PEP_STATUS messageToSend(struct _message *msg)
{
    TESTLOG("called()");
    return PEP_STATUS_OK;
}

PEP_STATUS notifyHandshake(::pEp_identity *me, ::pEp_identity *partner, ::sync_handshake_signal signal)
{
    TESTLOG("called()");
    return PEP_STATUS_OK;
}

int main(int argc, char **argv)
{
    Test::setup(argc, argv);
    Adapter::pEpLog::set_enabled(true);

    Adapter::session.initialize(Adapter::SyncModes::Async, false, messageToSend, notifyHandshake);
    // Create new identity
    TESTLOG("updating or creating identity for me");
    ::pEp_identity *me = new_identity("alice@peptest.ch", NULL, "23", "Who the F* is Alice");
    assert(me);
    ::PEP_STATUS status = ::myself(Adapter::session(), me);
    ::free_identity(me);
    pEp::throw_status(status);

    // start and stop sync repeatedly
    useconds_t sleepuSec = 1000 * 100;
    unsigned long long int nrIters = 1000 * 1000 * 1000;
    for (int i = 0; i < nrIters; i++) {
        TESTLOG("RUN NR: ");
        TESTLOG(i);
        TESTLOG("SYNC START");
        TESTLOG("starting the adapter including sync");
        Adapter::startup();
        TESTLOG("SYNC STOP");
        usleep(sleepuSec);
        Adapter::stop_sync();
    }
    return 0;
}
