#include "PityModel.hh"
#include "PityUnit.hh"
#include "PitySwarm.hh"

#include <vector>
#include <functional>
#include <memory>

namespace pEp {
    namespace PityTest11 {
        bool PitySwarm::debug_log_enabled = false;

        void createPerspective(const PityModel& model, PityPerspective* psp, int node_nr)
        {
            psp->name = model.nodeNr(node_nr)->getName();

            // Default partner is next node, its a circle
            int partner_node_index = (node_nr + 1) % model.nodes().size();
            psp->partner = model.nodes().at(partner_node_index)->getName();

            // Create peers, everyone but me
            auto nodes = model.nodes();
            for (int i = 0; i < nodes.size(); i++) {
                if (i != node_nr) {
                    psp->peers.push_back(nodes.at(i)->getName());
                }
            }
        }

        PitySwarm::PitySwarm(PityModel& model) : _model{ model }
        {
            pEpLogClass("called");
            // Create perspective
            for (auto n : _model.nodes()) {
                auto tmp = std::make_shared<PityPerspective>();
                createPerspective(_model, tmp.get(), n->getNr());
                _perspectives.push_back(tmp);
            }

            // Construct Tree
            _rootUnit = std::make_shared<PityUnit<PityModel, PityPerspective>>(
                nullptr,
                _model.getName(),
                nullptr,
                &_model);

            for (auto n : _model.nodes()) {
                _nodeUnits.push_back(std::make_shared<PityUnit<>>(
                    _rootUnit.get(),
                    n->getName(),
                    nullptr,
                    //                    std::bind(
                    //                        &PityNode::_init,
                    //                        this,
                    //                        std::placeholders::_1,
                    //                        std::placeholders::_2,
                    //                        std::placeholders::_3),
                    &_model,
                    _perspectives.at(n->getNr()).get(),
                    PityUnit<>::ExecutionMode::PROCESS_PARALLEL));
            }
        }

        void PitySwarm::addTestUnit(
            int nodeNr,
            const std::string& name,
            std::function<void(PityUnit<>&, PityModel*, PityPerspective*)> test_func)
        {
            auto tmp = std::make_shared <PityUnit<>>(_nodeUnits.at(nodeNr).get(), name, test_func);
            _testUnits.push_back(tmp);
        }



        void PitySwarm::run()
        {

            _rootUnit->run();
        }


    } // namespace PityTest11
} // namespace pEp
