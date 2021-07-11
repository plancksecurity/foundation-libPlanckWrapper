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
            _model{ model }, _swarmUnit{ name,
                                         nullptr,
                                         nullptr,
                                         PityUnit<>::ExecutionMode::PROCESS_SEQUENTIAL }
        {
            logger_debug.set_instancename(name);
            pEpLogClass("called");

            for (auto n : _model.nodes()) {
                TestUnit* tmp = &_swarmUnit.addNew<TestUnit>(
                    n->getName(),
                    std::bind(
                        &PitySwarm::_init_process,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2),
                    nullptr,
                    TestUnit::ExecutionMode::PROCESS_PARALLEL);
                // By value copies the context into the TestUnit
                tmp->setContext(_createPerspective(_model, n->getNr()));
                _nodeUnits.insert(std::pair<int, TestUnit*>(n->getNr(), tmp));
            }
        }

        PitySwarm::PitySwarm(const PitySwarm& rhs, const std::string& new_name) :
            _model{ rhs._model }, _swarmUnit{ new_name }
        {
            logger_debug.set_instancename(new_name);
            _swarmUnit = TestUnit(rhs._swarmUnit);
            // TODO: Hack for some reason ExecMode is getting copied,
            // Copy of Swarm is _b0rken
            _swarmUnit.setExecMode(PityUnit<>::ExecutionMode::PROCESS_SEQUENTIAL);
            _swarmUnit.setName(new_name);
            for (auto n : rhs._nodeUnits) {
                TestUnit* tmp = &_swarmUnit.addCopy(TestUnit(*n.second));
                _nodeUnits.insert(std::pair<int, TestUnit*>(n.first, tmp));
            }
        }

        PitySwarm::TestUnit& PitySwarm::getSwarmUnit()
        {
            return _swarmUnit;
        }

        PitySwarm::TestUnit& PitySwarm::getLeafUnit(int nodeNr)
        {
            TestUnit* ret = nullptr;
            TestUnit* current = _nodeUnits.at(nodeNr);
            do {
                if (current == nullptr) {
                    throw std::runtime_error("bad fatal cast in the ugly hack");
                }
                if (current->getChildCount() == 0) {
                    ret = current;
                } else {
                    current = dynamic_cast<TestUnit*>(
                        &(current->getChildRefs().begin()->second)); // random child
                }
            } while (ret == nullptr);
            return *ret;
        }

        PitySwarm::TestUnit& PitySwarm::addTestUnit(int nodeNr, const TestUnit& unit)
        {
            TestUnit& ret = getLeafUnit(nodeNr).addCopy(std::move(unit));
            return ret;
        }

        void PitySwarm::run()
        {
            _swarmUnit.run();
        }

        // The perspective currently is completely defined by specifying a node,
        //  since there is a 1-1 node/ident relationship currently
        PityPerspective PitySwarm::_createPerspective(const PityModel& model, int node_nr)
        {
            PityPerspective psp{ model };
            psp.own_name = model.nodeNr(node_nr)->getName();

            // Create peers, everyone but me
            for (int i = 0; i < model.nodes().size(); i++) {
                if (i != node_nr) {
                    psp.peers.push_back(TestIdent(model.nodes().at(i)->getIdent()));
                }
            }

            // Default partner is next node, its a circle
            //            int partner_node_index = (node_nr + 1) % model.nodes().size();
            //            psp.cpt_name = model.nodes().at(partner_node_index)->getName();

            //Default partner is node 0
            if(model.nodes().size() > 1) {
                if (node_nr == 0) {
                    psp.setPeerNrAsCpt(0);
                } else {
                    for (int i = 0; i < psp.peers.size(); i++) {
                        if (psp.peers.at(i).addr == model.nodeNr(0)->getIdent().addr) {
                            psp.setPeerNrAsCpt(i);
                        }
                    }
                }
            }

            // Groups
            int grpOwneNode = 0;
            Group grp1 = Group{};
            grp1.addr = "grp_" + model.nodeNr(grpOwneNode)->getName();
            grp1.moderator = model.nodeNr(grpOwneNode)->getName();
            // Create peers, everyone but me
            for (int i = 0; i < model.nodes().size(); i++) {
                if (i != grpOwneNode) {
                    grp1.members.push_back(TestIdent(model.nodes().at(i)->getIdent()));
                }
            }
            psp.groups.push_back(grp1);

            return psp;
        }

        int PitySwarm::_init_process(TestUnit& unit, PityPerspective* ctx)
        {
            // This should not be needed
            //            std::cout << "Node _initProcUnitNrRecurse, setting $HOME" << std::endl;
            //            std::string home = unit.processDir();
            //            setenv("HOME", home.c_str(), true);
            return 0;
        }
    } // namespace PityTest11
} // namespace pEp
