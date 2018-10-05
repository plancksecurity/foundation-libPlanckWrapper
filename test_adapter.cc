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
    cout << "calling messageToSend();\n";   
    return PEP_STATUS_OK;
}

PEP_STATUS notifyHandshake(void *obj, pEp_identity *me, pEp_identity *partner, sync_handshake_signal signal)
{
    cout << "calling notifyHandshake();\n";   
    return PEP_STATUS_OK;
}

int main()
{
    cout << "updating or creating identity for me\n";
    pEp_identity *me = new_identity("alice@peptest.ch", NULL, "23", "Who the F* is Alice");
    assert(me);
    PEP_STATUS status = myself(session(), me);
    free_identity(me);
    throw_status(status);

    cout << "starting the adapter including sync\n";
    startup(messageToSend, notifyHandshake);
    sleep(3);
    shutdown();

    return 0;
}

