// This file is under GNU General Public License 3.0
// see LICENSE.txt

#pragma once

#include "locked_queue.hh"
#include <thread>
#include <string>
#include <pEp/sync_api.h>

using namespace std;

namespace pEp {
    void throw_status(PEP_STATUS status);

    struct RuntimeError : runtime_error {
        RuntimeError(string _text, PEP_STATUS _status);
        string text;
        PEP_STATUS status;
    };

    namespace Adapter {
        void startup(messageToSend_t messageToSend,
                notifyHandshake_t notifyHandshake, void *obj = nullptr);

        enum session_action {
            init,
            release
        };
        PEP_SESSION session(session_action action = init);

        void shutdown();
    }
}

