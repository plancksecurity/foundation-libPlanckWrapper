include ../Makefile.conf

LDFLAGS:=-L../src $(LDFLAGS) -L./pitytest11/src
LDLIBS=-lstdc++ -lpEpEngine -lpEpAdapter -lPityTest
CXXFLAGS:=-I../src -DENGINE_TEST=$(ENGINE_TEST) $(CXXFLAGS)

# Test
SRC_TEST=$(wildcard test_*.cc)
BIN_TEST=$(subst .cc,,$(SRC_TEST))

# Framework
SRC_FRAMEWORK=$(wildcard framework/*.cc)
OBJ_FRAMEWORK=$(subst .cc,.o,$(SRC_FRAMEWORK))

# Depends
#DEP_TEST=$(subst .cc,.d,$(SRC_TEST))
#DEP_FRAMEWORK=$(subst .cc,.d,$(SRC_FRAMEWORK))
#DEPENDS+=$(DEP_TEST)$(DEP_FRAMEWORK)
#CXXFLAGS+= -MMD -MP
#
#ifneq ($(MAKECMDGOALS),clean)
#    -include $(DEPENDS)
#endif

.PHONY: all clean rmtestdata pitytest pitytest-clean
.DEFAULT_GOAL := all

all: $(BIN_TEST)

$(BIN_TEST): $(OBJ_FRAMEWORK)

pitytest:
	$(MAKE) -C pitytest11

clean:
	rm -f $(BIN_TEST)
	rm -f $(OBJ_FRAMEWORK)
	rm -Rf *.dSYM
	rm -f *.d
	rm -f *.o
	rm -Rf /tmp/test_pEp.*

pitytest-clean:
	$(MAKE) -C pitytest11 clean

rmtestdata:
	rm -Rf /tmp/test_pEp.*
