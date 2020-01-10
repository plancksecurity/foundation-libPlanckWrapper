// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "Adapter.hh"
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <pEp/keymanagement.h>

using namespace std;
using namespace pEp::Adapter;

PEP_STATUS messageToSend(struct _message *msg)
{
    pEpLog("called()");
    return PEP_STATUS_OK;
}

PEP_STATUS notifyHandshake(pEp_identity *me, pEp_identity *partner, sync_handshake_signal signal)
{
    pEpLog("called()");
    return PEP_STATUS_OK;
}

int main()
{
    pEpLog("updating or creating identity for me");
    pEp_identity *me = new_identity("alice@peptest.ch", NULL, "23", "Who the F* is Alice");
    assert(me);
    PEP_STATUS status = myself(session(), me);
    free_identity(me);
    pEp::throw_status(status);

    pEpLog("starting the adapter including sync");
    startup(messageToSend, notifyHandshake);
    sleep(3);
    shutdown();

    return 0;
}
