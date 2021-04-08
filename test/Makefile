include ../Makefile.conf

LDFLAGS=-L../src $(ENGINE_LIB)
LDLIBS=-lstdc++ -lpEpEngine -lpEpAdapter
CXXFLAGS:=-I../src -DENGINE_TEST=$(ENGINE_TEST) $(CXXFLAGS)

SRC_TEST=$(wildcard test_*.cc)
BIN_TEST=$(subst .cc,,$(SRC_TEST))

SRC_FRAMEWORK=$(wildcard framework/*.cc)
OBJ_FRAMEWORK=$(subst .cc,.o,$(SRC_FRAMEWORK))

.PHONY: all clean rmtestdata

all: $(BIN_TEST)


$(BIN_TEST): $(OBJ_FRAMEWORK)


clean:
	rm -f $(BIN_TEST)
	rm -f $(OBJ_FRAMEWORK)
	rm -Rf *.dSYM
	rm -f *.o
	rm -Rf /tmp/test_pEp.*

rmtestdata:
	rm -Rf /tmp/test_pEp.*

