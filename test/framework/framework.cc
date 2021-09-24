// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <exception>
#include <thread>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>

#include <pEp/keymanagement.h>
#include <pEp/mime.h>
#include <pEp/message_api.h>
#include <pEp/sync_codec.h>
#include <pEp/distribution_codec.h>

#include "../../src/Adapter.hh"

pEp::Test::Transport pEp::Test::transport;
std::string pEp::Test::path;
extern std::thread pEp::Adapter::_sync_thread;

using namespace pEp;
using namespace std;

namespace pEp {
    namespace Test {
        void setup(vector<string> &args)
        {
#ifdef WIN32
            string dir = getenv("TEMP");
            dir += "\\test_pEp.XXXXXXXXXXXX";
#else
            string dir = "/tmp/test_pEp.XXXXXXXXXXXX";
#endif

            if (args.size() > 1) {
                if (args[1] == "--help") {
#ifdef WIN32
                    cout << "usage: " << args[0] << " [--dir LOCALAPPDATA]" << endl;
#else
                    cout << "usage: " << args[0] << " [--dir HOME]" << endl;
#endif
                    exit(0);
                } else if (args[1] == "--dir" && args.size() == 3) {
                    dir = args[2];
                } else {
                    cerr << "illegal parameter" << endl;
                    exit(1);
                }
            }

            char _path[MAXPATHLEN + 1];
            const char *templ = dir.c_str();
            strcpy(_path, templ);
            mkdtemp(_path);
            chdir(_path);
#ifdef WIN32
            setenv("LOCALAPPDATA", _path, 1);
#else
            setenv("HOME", _path, 1);
#endif
            path = _path;
            cerr << "test directory: " << path << endl;
        }

        void setup(int argc, char **argv)
        {
            vector<string> args{ (size_t)argc };
            for (int i = 0; i < argc; ++i) {
                args[i] = argv[i];
            }

            setup(args);
        }

        void import_key_from_file(string filename)
        {
            ifstream f(filename, ifstream::in);
            string key{ istreambuf_iterator<char>(f), istreambuf_iterator<char>() };
            ::identity_list *il = NULL;
            cout << key.c_str() << endl;
            cout << key.length() << endl;
            ::PEP_STATUS status = ::import_key(Adapter::session(), key.c_str(), key.length(), &il);
            throw_status(status);
            assert(status == PEP_KEY_IMPORTED);
            ::free_identity_list(il);
        }

        Message make_message(::message *msg)
        {
            return shared_ptr<::message>(msg, ::free_message);
        }

        Identity make_identity(::pEp_identity *ident)
        {
            return shared_ptr<::pEp_identity>(ident, ::free_identity);
        }

        Message mime_parse(string text)
        {
            ::message *msg;
            bool has_possible_pEp_msg;
            ::PEP_STATUS status = ::mime_decode_message(
                text.c_str(),
                text.length(),
                &msg,
                &has_possible_pEp_msg);
            throw_status(status);
            return make_message(msg);
        }

        string mime_compose(Message msg)
        {
            char *mimetext;
            PEP_STATUS status = ::mime_encode_message(msg.get(), false, &mimetext, false);
            throw_status(status);
            string text = mimetext;
            free(mimetext);
            return text;
        }

        string make_pEp_msg(Message msg)
        {
            string text;

            ::message *_dst;
            stringlist_t *keylist;
            ::PEP_decrypt_flags_t flags = 0;
            ::PEP_STATUS status = ::decrypt_message(Adapter::session(), msg.get(), &_dst, &keylist, &flags);
            throw_status(status);

            Message dst;
            if (_dst)
                dst = make_message(_dst);
            else
                dst = msg;
            ::PEP_rating rating = dst.get()->rating;

            if (dst.get()->attachments) {
                for (auto a = dst.get()->attachments; a && a->value; a = a->next) {
                    if (string("application/pEp.sync") == a->mime_type) {
                        char *_text;
                        status = ::PER_to_XER_Sync_msg(a->value, a->size, &_text);
                        throw_status(status);
                        text += _text;
                        ::pEp_free(_text);
                        return text;
                    } else if (string("application/pEp.distribution") == a->mime_type) {
                        char *_text;
                        status = ::PER_to_XER_Distribution_msg(a->value, a->size, &_text);
                        throw_status(status);
                        text += _text;
                        ::pEp_free(_text);
                        return text;
                    }
                }
            }

            return text;
        }

        void join_sync_thread()
        {
            if (Adapter::_sync_thread.joinable()) {
                Adapter::_sync_thread.join();
            }
        }

        Message Transport::recv()
        {
            mkdir(inbox_path.c_str(), 0770);
            auto msg = make_message(nullptr);

            return msg;
        }

        void Transport::send(Message msg)
        {
            mkdir(outbox_path.c_str(), 0770);
        }
    }; // namespace Test
};     // namespace pEp
