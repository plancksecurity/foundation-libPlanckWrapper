#include "Adapter.hh"
#include <iostream>
#include <assert.h>
#include <unistd.h>

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
    Adapter(messageToSend, notifyHandshake);
    sleep(3);
    Adapter::shutdown();

    return 0;
}

