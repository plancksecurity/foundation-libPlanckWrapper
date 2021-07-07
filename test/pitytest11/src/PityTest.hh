// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYTEST_HH
#define PITYTEST_PITYTEST_HH

#include "PityUnit.hh"
#include "PityModel.hh"
#include "PitySwarm.hh"
#include "PityPerspective.hh"

#ifndef PITYASSERT
    #define PITYASSERT(condition, msg)                                                             \
        do {                                                                                       \
            if (!(condition)) {                                                                    \
                throw PityAssertException(msg);                                                    \
            }                                                                                      \
        } while (0);
#endif

#define PITYASSERT_THROWS(func, msg)                                                               \
    do {                                                                                           \
        try {                                                                                      \
            (func);                                                                                \
            PITYASSERT(false, msg);                                                                \
        } catch (const PityAssertException& pae) {                                                 \
            throw(pae);                                                                            \
        } catch (const std::exception& e) {                                                        \
        } catch (...) {                                                                            \
        }                                                                                          \
    } while (0);
#endif