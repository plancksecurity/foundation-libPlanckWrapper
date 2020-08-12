#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Adapter.hh"

namespace pEp {
    namespace Test {
        using namespace std;

        // manually set up test
        void setup(vector<string>& args);

        // call this in main() for auto set up
        void setup(int argc=1, char **argv=nullptr);

        void import_key_from_file(string filename);

        using Message = shared_ptr<::message>;

        // use this instead of constructor to auto assign ::free_message as
        // deleter
        Message make_message(::message *msg);

        // MIME parser
        Message make_message(string text);

        // MIME composer
        string make_string(Message msg);

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
    };
};

