// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef LIBPEPADAPTER_FRAMEWORK_HH
#define LIBPEPADAPTER_FRAMEWORK_HH

#include <string>
#include <vector>
#include <memory>

#include <pEp/message.h>

namespace pEp {
    namespace Test {
        // manually set up test
        void setup(std::vector<std::string> &args);

        // call this in main() for auto set up
        void setup(int argc = 1, char **argv = nullptr);

        std::string get_resource_abs(const std::string &name);

        void import_key_from_file(const std::string& filename);

        using Message = std::shared_ptr<::message>;
        using Identity = std::shared_ptr<::pEp_identity>;

        // use this instead of constructor to auto assign ::free_message as
        // deleter
        Message make_message(::message *msg);

        // use this instead of constructor to auto assign ::free_identity as
        // deleter
        Identity make_identity(::pEp_identity *ident);

        // MIME parser
        Message mime_parse(std::string text);

        // MIME composer
        std::string mime_compose(Message msg);

        // Sync and Distribution decoder
        std::string make_pEp_msg(Message msg);

        // wait until Sync has shut down
        void join_sync_thread();

        struct Transport {
            std::string inbox_path = "inbox";
            std::string outbox_path = "outbox";

            // reads next message from inbox
            Message recv();

            // appends message to outbox
            void send(Message msg);
        };

        extern Transport transport;
        extern std::string per_user_dir;
    }; // namespace Test
};     // namespace pEp

#endif // LIBPEPADAPTER_FRAMEWORK_HH
