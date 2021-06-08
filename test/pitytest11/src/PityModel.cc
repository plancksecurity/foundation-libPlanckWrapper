#include "PityModel.hh"
//#include "PityUnit.hh"
#include "iostream"
#include "../../../src/std_utils.hh"
#include <random>
#include <fstream>
#include <memory>

namespace pEp {
    namespace PityTest11 {
        bool PityModel::debug_log_enabled = false;

        PityModel::PityModel(const std::string& name, int nodeCount) :
            _name{ name }, _unit{ nullptr, name, nullptr, this }
        {
            for (int i = 0; i < nodeCount; i++) {
                _nodes.emplace_back(std::make_shared<PityNode>(*this, i));
            }

        }

        std::string PityModel::getName() const
        {
            return _name;
        }

        void PityModel::setName(std::string name)
        {
            _name = name;
        }

        std::vector<std::shared_ptr<PityNode>> PityModel::nodes() const
        {
            return _nodes;
        }

        PityUnit<PityModel>& PityModel::unit()
        {
            return _unit;
        }

        PityUnit<PityModel>* PityModel::unitOfNodeNr(int nr) const
        {
            return nodes().at(nr)->unit().get();
        }

        PityNode* PityModel::nodeNr(int nr) const
        {
            return nodes().at(nr).get();
        }

        void PityModel::run()
        {
            unit().run();
        }

        void PityModel::sendMsg(const std::string nodename, const std::string& msg) const
        {
            pEpLogClass("Address: " + nodename + " msg: " + msg);
            bool found = false;
            for (auto n : _nodes) {
                if (n->getName() == nodename) {
                    found = true;
                    Utils::dir_ensure(n->inboxDir());
                    std::stringstream filename;
                    // collision detect
                    do {
                        filename << n->inboxDir() << Utils::random_string(97, 122, 16) << ".pitymsg";
                    } while (Utils::path_exists(filename.str()));
                    std::ofstream msgfile = Utils::file_create(filename.str());
                    msgfile << msg;
                }
            }
            if (!found) {
                throw std::runtime_error("no such nodename: " + nodename);
            }
        }

        bool PityModel::hasMsg() const{
            bool ret = false;
            pEpLogClass("called");
            Utils::dir_ensure(own_node->inboxDir());
            auto msg_filenames = Utils::dir_list_files(own_node->inboxDir());
            ret = msg_filenames.size() > 0;
            return ret;
        }

        // Non-blocking
        // throws underflow_error if inbox empty
        std::string PityModel::pollMsg() const
        {
            pEpLogClass("called");
            std::string ret;
            Utils::dir_ensure(own_node->inboxDir());
            auto msg_filenames = Utils::dir_list_files(own_node->inboxDir());
            if (!msg_filenames.empty()) {
                std::string msg_filename = msg_filenames.at(0);
                std::string msg_path = own_node->inboxDir() + "/" + msg_filename;
                pEpLogClass("Reading file: " + msg_filename);
                ret = Utils::file_read(msg_path);
                Utils::path_delete(msg_path);
            } else {
                throw std::underflow_error("inbox empty: " + own_node->inboxDir());
            }

            return ret;
        }

        std::string PityModel::receiveMsg(int timeout_msec) const
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
