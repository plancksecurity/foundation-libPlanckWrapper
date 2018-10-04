// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "Adapter.hh"
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <pEp/keymanagement.h>

using namespace pEp;
using namespace std;

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
    PEP_STATUS status = myself(Adapter::session(), me);
    throw_status(status);

    cout << "starting the adapter including sync\n";
    Adapter(messageToSend, notifyHandshake);
    sleep(3);
    Adapter::shutdown();

    free_identity(me);
    return 0;
}

