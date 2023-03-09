// This file is under GNU General Public License 3.0
// see LICENSE.txt

#include "framework.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <exception>
#include <thread>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <sys/param.h>
#include <sys/stat.h>
#include <pEp/pEpLog.hh>
#include <pEp/keymanagement.h>
#include <pEp/mime.h>
#include <pEp/message_api.h>
#include <pEp/sync_codec.h>
#include <pEp/distribution_codec.h>

#include "../../src/Adapter.hh"

pEp::Test::Transport pEp::Test::transport;

using namespace pEp;

namespace pEp {
    namespace Test {
        std::string per_user_dir{};
        std::string resource_dir{};

        void setup(std::vector<std::string> &args)
        {
            std::string dir{};
            resource_dir = Utils::path_get_abs("resource/");
#ifdef WIN32
            dir = getenv("TEMP");
            dir += "\\test_pEp.XXXXXXXXXXXX";
#endif

            if (args.size() > 1) {
                if (args[1] == "--help") {
#ifdef WIN32
                    std::cout << "usage: " << args[0] << " [--dir LOCALAPPDATA]" << std::endl;
#else
                    std::cout << "usage: " << args[0] << " [--dir HOME]" << std::endl;
#endif
                    exit(0);
                } else if (args[1] == "--dir" && args.size() == 3) {
                    dir = args[2];
                } else {
                    std::cerr << "illegal parameter" << std::endl;
                    exit(1);
                }
            }

#ifdef WIN32
            char _path[MAXPATHLEN + 1];
            const char *templ = dir.c_str();
            strcpy(_path, templ);
            mkdtemp(_path);
            chdir(_path);
            setenv("LOCALAPPDATA", _path, 1);
            per_user_dir = _path;
#else
            dir = Utils::path_get_abs("testdata");
            Utils::dir_recreate(dir);
            setenv("HOME", dir.c_str(), 1);
            Utils::dir_set_cwd(dir);
            pEpLog("$HOME set to:" + dir);
            per_user_dir = dir;
#endif

            std::cerr << "test directory: " << per_user_dir << std::endl;
        }

        void setup(int argc, char **argv)
        {
            std::vector<std::string> args{ (size_t)argc };
            for (int i = 0; i < argc; ++i) {
                args[i] = argv[i];
            }

            setup(args);
        }

        std::string get_resource_abs(const std::string &name)
        {
            return Utils::path_get_abs(resource_dir + name);
        }

        void import_key_from_file(const std::string &filename)
        {
            std::string key = Utils::file_read(filename);
            //            ifstream f(filename, ifstream::in);
            //            string key{ istreambuf_iterator<char>(f), istreambuf_iterator<char>() };
            ::identity_list *il = nullptr;
            std::cout << key.c_str() << std::endl;
            std::cout << key.length() << std::endl;
            ::PEP_STATUS status = ::import_key(Adapter::session(), key.c_str(), key.length(), &il);
            throw_status(status);
            assert(status == PEP_KEY_IMPORTED);
            ::free_identity_list(il);
        }

        Message make_message(::message *msg)
        {
            return std::shared_ptr<::message>(msg, ::free_message);
        }

        Identity make_identity(::pEp_identity *ident)
        {
            return std::shared_ptr<::pEp_identity>(ident, ::free_identity);
        }

        Message mime_parse(std::string text)
        {
            ::message *msg = nullptr;
            bool has_possible_pEp_msg = false;
            ::PEP_STATUS status = ::mime_decode_message(
                text.c_str(),
                text.length(),
                &msg,
                &has_possible_pEp_msg);
            throw_status(status);
            return make_message(msg);
        }

        std::string mime_compose(Message msg)
        {
            char *mimetext = nullptr;
            PEP_STATUS status = ::mime_encode_message(msg.get(), false, &mimetext, false);
            throw_status(status);
            std::string text = mimetext;
            free(mimetext);
            return text;
        }

        std::string make_pEp_msg(Message msg)
        {
            std::string text;

            ::message *_dst = nullptr;
            stringlist_t *keylist = nullptr;
            ::PEP_rating rating{};
            ::PEP_decrypt_flags_t flags = 0;
            ::PEP_STATUS status = ::decrypt_message(
                Adapter::session(),
                msg.get(),
                &_dst,
                &keylist,
                &rating,
                &flags);
            throw_status(status);

            Message dst;
            if (_dst) {
                dst = make_message(_dst);
            } else {
                dst = msg;
            }

            if (dst.get()->attachments) {
                for (auto a = dst.get()->attachments; a && a->value; a = a->next) {
                    if (std::string("application/pEp.sync") == a->mime_type) {
                        char *_text;
                        status = ::PER_to_XER_Sync_msg(a->value, a->size, &_text);
                        throw_status(status);
                        text += _text;
                        ::pEp_free(_text);
                        return text;
                    } else if (std::string("application/pEp.distribution") == a->mime_type) {
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
    } // namespace Test
}     // namespace pEp
