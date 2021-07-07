#include "../src/PityTest.hh"
#include <iostream>
#include <exception>

using namespace pEp::PityTest11;

using TestUnit = PityUnit<>;

void not_throwing() {}

void throwing()
{
    throw std::runtime_error{ "Fsd" };
}

int main(int argc, char* argv[])
{
    TestUnit asserts {"test_asserts"};
    asserts.add("nfdsg", []() {
      PITYASSERT(true, "thats wrong");
      try {
          PITYASSERT(false, "thats correct");
          throw std::runtime_error("PITYASSERT(false) does not throw");
      } catch (const PityAssertException& pae) {
      }
    })


    PITYASSERT_THROWS(throwing(), "is actually throwing ");
    try {
        PITYASSERT_THROWS(not_throwing(), "is actually not throwing");
        throw std::runtime_error("PITYASSERT(false) does not throw");
    } catch (const PityAssertException& pae) {
    }
}
