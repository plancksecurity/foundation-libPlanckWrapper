#include "pitytest11/src/PityTest.hh"
#include "../src/utils.hh"
#include "framework/framework.hh"
#include "framework/utils.hh"

#include <pEp/pEpEngine.h>
#include <pEp/message_api.h>
#include <pEp/keymanagement.h>
#include <pEp/identity_list.h>
#include <pEp/Adapter.hh>
#include <pEp/status_to_string.hh>
#include <pEp/mime.h>

using namespace pEp;
using namespace pEp::Adapter;
using namespace pEp::Test::Utils;
using namespace pEp::PityTest11;

// Test template for 3 nodes (processes) running the same 2 tests each in their own environment in parallel

// This is the 1st test unit
int test_func1(PityUnit<PityPerspective> &pity, PityPerspective *ctx)
{
    pity.log(ctx->own_name);
    pity.log("getName: " + pity.getName());
    pity.log("getPath: " +pity.getPath());
    pity.log("getPathShort: " +pity.getPathShort());
    pity.log("transportDir: " +pity.transportDir());
    pity.log("processDir: " +pity.processDir());
    pity.log("getGlobalRootDir: " +pity.getGlobalRootDir());
    pity.log("to_string: " +pity.to_string());

    PITYASSERT(true,"");
    return 0;
}

// This is the 2nd test unit
int test_func2(PityUnit<PityPerspective> &pity, PityPerspective *ctx)
{
    pity.log(ctx->own_name);
    PITYASSERT(false,"");
    return 0;
}


int main(int argc, char *argv[])
{
    int nodecount = 3;
    PityModel model{ "templ_swarm_multi", nodecount };
    PitySwarm swarm{ model };

    // 1. Create the swarm process TestUnit
    // 2. Append a PityUnit to process 1 unit
    auto unit1 = swarm.addTestUnit(0, "unit1", test_func1);
    auto unit1_1 = PityUnit<PityPerspective>(*unit1,"unit1_1", test_func2);
    
    auto unit2 = swarm.addTestUnit(1, "unit2", test_func1);
    auto unit2_1 = PityUnit<PityPerspective>(*unit2,"unit2_1", test_func2);

    auto unit3 = swarm.addTestUnit(2, "unit3", test_func1);
    auto unit3_1 = PityUnit<PityPerspective>(*unit3,"unit3_1", test_func2);

    swarm.run();
}