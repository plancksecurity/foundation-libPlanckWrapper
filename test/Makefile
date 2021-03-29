include ../Makefile.conf

LDFLAGS=-L../src $(ENGINE_LIB)
LDLIBS=-lstdc++ -lpEpEngine -lpEpAdapter
CXXFLAGS:=-I../src -DENGINE_TEST=$(ENGINE_TEST) $(CXXFLAGS)
SRC=$(wildcard test_*.cc)
TST=$(subst .cc,,$(SRC))

all: $(TST)

$(TST): framework.o

.PHONY: clean rmtestdata

clean:
	rm -f $(TST)
	rm -Rf *.dSYM
	rm -f *.o
	rm -Rf /tmp/test_pEp.*

rmtestdata:
	rm -Rf /tmp/test_pEp.*

