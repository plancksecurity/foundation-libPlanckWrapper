include ../Makefile.conf

LDFLAGS=-L../ $(ENGINE_LIB)
LDLIBS=-lstdc++ -lpEpEngine -lpEpAdapter
CXXFLAGS+=-I../ $(ENGINE_INC) -std=c++11 -DENGINE_TEST=$(ENGINE_TEST)

SRC=$(wildcard test_*.cc)
TST=$(subst .cc,,$(SRC))

all: $(TST)

%: %.cc ../libpEpAdapter.a

clean:	
	rm -f $(TST)
	rm -Rf *.dSYM
