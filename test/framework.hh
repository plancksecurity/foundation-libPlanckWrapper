// This file is under GNU General Public License 3.0
// see LICENSE.txt
#ifndef LIBPEPADAPTER_FRAMEWORK_HH
#define LIBPEPADAPTER_FRAMEWORK_HH

#include <string>
#include <vector>
#include <memory>

#include "Adapter.hh"

namespace pEp {
    namespace Test {
        using namespace std;

        // manually set up test
        void setup(vector<string> &args);

        // call this in main() for auto set up
        void setup(int argc = 1, char **argv = nullptr);

        void import_key_from_file(string filename);

        using Message = shared_ptr<::message>;
        using Identity = shared_ptr<::pEp_identity>;

        // use this instead of constructor to auto assign ::free_message as
        // deleter
        Message make_message(::message *msg);

        // use this instead of constructor to auto assign ::free_identity as
        // deleter
        Identity make_identity(::pEp_identity *ident);

        // MIME parser
        Message mime_parse(string text);

        // MIME composer
        string mime_compose(Message msg);

        // Sync and Distribution decoder
        string make_pEp_msg(Message msg);

        // wait until Sync has shut down
        void join_sync_thread();

        struct Transport {
            string inbox_path = "inbox";
            string outbox_path = "outbox";

            // reads next message from inbox
            Message recv();

            // appends message to outbox
            void send(Message msg);
        };

        extern Transport transport;
        extern string path;
    }; // namespace Test
};     // namespace pEp

#endif // LIBPEPADAPTER_FRAMEWORK_HH
