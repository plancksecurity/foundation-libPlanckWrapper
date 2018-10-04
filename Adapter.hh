// This file is under GNU General Public License 3.0
// see LICENSE.txt

#pragma once

#include "locked_queue.hh"
#include <thread>
#include <pEp/sync_api.h>

namespace pEp {
    void throw_status(PEP_STATUS status);

    class Adapter {
            static messageToSend_t _messageToSend;
            static notifyHandshake_t _notifyHandshake;
            static std::thread *_sync_thread;

        public:
            Adapter(messageToSend_t messageToSend,
                    notifyHandshake_t notifyHandshake);
            virtual ~Adapter() { }

            enum session_action {
                init,
                release
            };

            static PEP_SESSION session(session_action action = init);
            static ::utility::locked_queue< SYNC_EVENT >& queue()
            {
                static ::utility::locked_queue< SYNC_EVENT > q;
                return q;
            }

        protected:
            static int _inject_sync_event(SYNC_EVENT ev, void *management);
            static SYNC_EVENT _retrieve_next_sync_event(void *management, time_t threshold);
            static void sync_thread();

        private:
            static std::mutex& mtx()
            {
                static std::mutex m;
                return m;
            }
    };
}

