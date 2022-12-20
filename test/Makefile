include ../Makefile.conf

LDFLAGS:=-L../src $(LDFLAGS)
LDLIBS=-lstdc++ -lpEpEngine -lpEpAdapter -lpEpCxx11 -lPityTest11 -lpthread -ldl -lm
CXXFLAGS:=-I../src -DENGINE_TEST=$(ENGINE_TEST) $(CXXFLAGS)

# Test
SRC_TEST=$(wildcard test_*.cc)
BIN_TEST=$(subst .cc,,$(SRC_TEST))

# Framework
SRC_FRAMEWORK=$(wildcard framework/*.cc)
OBJ_FRAMEWORK=$(subst .cc,.o,$(SRC_FRAMEWORK))

.PHONY: all clean
.DEFAULT_GOAL := all

all: $(BIN_TEST)

$(BIN_TEST): $(OBJ_FRAMEWORK)

clean:
	rm -f $(BIN_TEST)
	rm -f $(OBJ_FRAMEWORK)
	rm -Rf *.dSYM
	rm -f *.d
	rm -f *.o
	rm -rf pitytest_data/


