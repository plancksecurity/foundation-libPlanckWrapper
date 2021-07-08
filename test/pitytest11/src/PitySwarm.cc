#include "PitySwarm.hh"
#include "PityModel.hh"
#include "PityPerspective.hh"
#include "PityUnit.hh"

#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include <stdlib.h>

namespace pEp {
    namespace PityTest11 {
        bool PitySwarm::debug_log_enabled = false;

        PitySwarm::PitySwarm(const std::string& name, PityModel& model) :
            _model{ model }, _swarmUnit(name)
        {
            pEpLogClass("called");
            // Create perspective
            for (auto n : _model.nodes()) {
                auto tmp = std::make_shared<PityPerspective>(model);
                _createPerspective(_model, tmp.get(), n->getNr());
                _perspectives.push_back(tmp);
            }

            // Construct swarm
            _swarmUnit = TestUnit(_model.getName(), nullptr, nullptr);

            for (auto n : _model.nodes()) {
                TestUnit* tmp = &_swarmUnit.addNew<TestUnit>(
                    n->getName(),
                    std::bind(
                        &PitySwarm::_init_process,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2),
                    _perspectives.at(n->getNr()).get(),
                    TestUnit::ExecutionMode::PROCESS_PARALLEL);
                _leafunit.insert(std::pair<int, TestUnit*>(n->getNr(), tmp));
            }
        }

        PitySwarm::TestUnit& PitySwarm::getSwarmUnit()
        {
            return _swarmUnit;
        }

        PitySwarm::TestUnit& PitySwarm::addTestUnit(int nodeNr, const TestUnit& unit)
        {
            TestUnit& ret = _leafunit.at(nodeNr)->addCopy(std::move(unit));
            _leafunit.at(nodeNr) = &ret;
            return ret;
        }

        void PitySwarm::run()
        {
            _swarmUnit.run();
        }

        // The perspective currently is complete defined by specifying a node, since there is a 1-1 node/ident relationship currently
        void PitySwarm::_createPerspective(const PityModel& model, PityPerspective* psp, int node_nr)
        {
            psp->own_name = model.nodeNr(node_nr)->getName();

            // Default partner is next node, its a circle
            int partner_node_index = (node_nr + 1) % model.nodes().size();
            psp->cpt_name = model.nodes().at(partner_node_index)->getName();

            // Create peers, everyone but me
            auto nodes = model.nodes();
            for (int i = 0; i < nodes.size(); i++) {
                if (i != node_nr) {
                    psp->peers.push_back(nodes.at(i)->getName());
                }
            }

            // Groups
            int grp_mod_node_nr = 0;
            if (grp_mod_node_nr == node_nr) {
                Test::Utils::Group grp1 = Test::Utils::Group{};
                grp1.name = "grp_" + psp->own_name;
                grp1.moderator = psp->own_name;
                grp1.members = psp->peers;
                psp->own_groups.push_back(grp1);
            }
        }

        int PitySwarm::_init_process(TestUnit& unit, PityPerspective* ctx)
        {
            std::cout << "Node _initProcUnitNrRecurse, setting $HOME" << std::endl;
            std::string home = unit.processDir();
            setenv("HOME", home.c_str(), true);
            return 0;
        }


    } // namespace PityTest11
} // namespace pEp
