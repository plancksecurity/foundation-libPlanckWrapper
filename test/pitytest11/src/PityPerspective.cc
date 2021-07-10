#include "PityPerspective.hh"
#include "PityModel.hh"

namespace pEp {
    namespace PityTest11 {
        bool PityPerspective::debug_log_enabled = false;

        PityPerspective::PityPerspective(const PityModel& model) : model{ model }, peerNrAsCpt{ 0 }
        {
            pEpLogClass("called");
        }

        TestIdent* PityPerspective::getPeer(const std::string& addr)
        {
            for (int i = 0; i < peers.size(); i++) {
                if (peers.at(i).addr == addr) {
                    return &peers.at(i);
                }
            }
            throw std::invalid_argument("getPeer(+" + addr + ") - not found");
        }

        void PityPerspective::setPeerNrAsCpt(int nr)
        {
            if (nr < peers.size()) {
                peerNrAsCpt = nr;
            } else {
                throw std::invalid_argument("setPeerNrAsCpt(" + std::to_string(nr) + ") - out of range");
            }
        }

        TestIdent& PityPerspective::getCpt()
        {
            return peers.at(peerNrAsCpt);
        }

        Group* PityPerspective::getGroup(const std::string& addr)
        {
            for (int i = 0; i < groups.size(); i++) {
                if (groups.at(i).addr == addr) {
                    return &groups.at(i);
                }
            }
            throw std::invalid_argument("getGroup(" + addr + ") - not found");
        }

    } // namespace PityTest11
} // namespace pEp
