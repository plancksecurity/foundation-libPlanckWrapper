// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework/framework.hh"
#include "framework/utils.hh"
#include <iostream>
#include <assert.h>

#include <pEp/keymanagement.h>
#include <pEp/sync_api.h>

#include "../src/Adapter.hh"

namespace Utils = pEp::Utils;
using namespace pEp;

PEP_STATUS messageToSend(struct _message *msg)
{
    TESTLOG("called");
    return PEP_STATUS_OK;
}

PEP_STATUS notifyHandshake(pEp_identity *me, pEp_identity *partner, ::sync_handshake_signal signal)
{
    TESTLOG("called");
    return PEP_STATUS_OK;
}

class JNISync {
public:
    void onSyncStartup()
    {
        TESTLOG("called");
    }

    void onSyncShutdown()
    {
        TESTLOG("called");
    }
} o;

int main(int argc, char* argv[])
{
    Test::setup(argc,argv);
    pEp::Adapter::session
        .initialize(pEp::Adapter::SyncModes::Async, false, messageToSend, notifyHandshake);
    // Create new identity
    TESTLOG("updating or creating identity for me");
    ::pEp_identity *me = ::new_identity("alice@peptest.ch", NULL, "23", "Who the F* is Alice");
    assert(me);
    ::PEP_STATUS status = ::myself(Adapter::session(), me);
    ::free_identity(me);
    throw_status(status);

    // start and stop sync repeatedly
    unsigned long long int nrIters = 3;
    for (int i = 0; i < nrIters; i++) {
        TESTLOG("RUN NR: ");
        TESTLOG(i);
        TESTLOG("SYNC START");
        TESTLOG("starting the adapter including sync");
        Adapter::startup<JNISync>(&o, &JNISync::onSyncStartup, &JNISync::onSyncShutdown);
        TESTLOG("SYNC STOP");
        Utils::sleep_millis(500);
        Adapter::stop_sync();
    }
    return 0;
}