#include "PityTransport.hh"
#include "PityUnit.hh"
#include "iostream"
#include "../../../src/std_utils.hh"
#include <random>
#include <fstream>
#include <memory>
#include <unordered_map>


namespace pEp {
    namespace PityTest11 {
        bool PityTransport::debug_log_enabled = false;

        PityTransport::PityTransport(std::string inboxDir, Endpoints& endpoints) :
            _inboxDir{ inboxDir }, _endpoints{ endpoints }
        {
        }

        void PityTransport::sendMsg(const std::string nodename, const std::string& msg) const
        {
            pEpLogClass("Address: " + nodename + " msg: " + msg);

            // HACK TODO
            std::string nodename_normalize = PityUnit<>::_normalizeName(nodename);

            bool found = false;
            std::string dir;
            try {
                dir = _endpoints.at(nodename_normalize);
            } catch (std::out_of_range&) {
                throw std::runtime_error("no such nodename: " + nodename_normalize);
            }
            Utils::dir_ensure(dir);
            std::stringstream filename;
            // collision detect
            do {
                filename << dir << Utils::random_string(97, 122, 16) << ".pitymsg";
            } while (Utils::path_exists(filename.str()));
            // create
            std::ofstream msgfile = Utils::file_create(filename.str());
            // write
            msgfile << msg;
        }

        bool PityTransport::hasMsg() const
        {
            bool ret = false;
            pEpLogClass("called");
            Utils::dir_ensure(_inboxDir);
            auto msg_filenames = Utils::dir_list_files(_inboxDir);
            ret = msg_filenames.size() > 0;
            return ret;
        }

        // Non-blocking
        // throws underflow_error if inbox empty
        std::string PityTransport::pollMsg() const
        {
            pEpLogClass("called");
            std::string ret;
            Utils::dir_ensure(_inboxDir);
            auto msg_filenames = Utils::dir_list_files(_inboxDir);
            if (!msg_filenames.empty()) {
                std::string msg_filename = msg_filenames.at(0);
                std::string msg_path = _inboxDir + "/" + msg_filename;
                pEpLogClass("Reading file: " + msg_filename);
                ret = Utils::file_read(msg_path);
                Utils::path_delete(msg_path);
            } else {
                throw std::underflow_error("inbox empty: " + _inboxDir);
            }

            return ret;
        }

        std::string PityTransport::receiveMsg(int timeout_msec) const
        {
            pEpLogClass("called");
            std::string ret;
            bool retry = false;
            do {
                try {
                    ret = pollMsg();
                    retry = false;
                } catch (const std::underflow_error&) {
                    pEpLogClass("polling again in [ms]: " + std::to_string(timeout_msec) + "...");
                    Utils::sleep_millis(timeout_msec);
                    retry = true;
                }
            } while (retry);
            return ret;
        }

    } // namespace PityTest11
} // namespace pEp
