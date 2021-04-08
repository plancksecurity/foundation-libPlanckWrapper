include ../Makefile.conf

LDFLAGS:=-L../src $(LDFLAGS)
LDLIBS=-lstdc++ -lpEpEngine -lpEpAdapter
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

.PHONY: all clean rmtestdata
.DEFAULT_GOAL := all

all: $(BIN_TEST)

$(BIN_TEST): $(OBJ_FRAMEWORK)

clean:
	rm -f $(BIN_TEST)
	rm -f $(OBJ_FRAMEWORK)
	rm -Rf *.dSYM
	rm -f *.d
	rm -f *.o
	rm -Rf /tmp/test_pEp.*

rmtestdata:
	rm -Rf /tmp/test_pEp.*
