include ../Makefile.conf

LDFLAGS=-L../ $(ENGINE_LIB)
LDLIBS=-lstdc++ -lpEpEngine -lpEpAdapter
CXXFLAGS+=-I../

.PHONY=all, test_adapter, test_library

all: test_adapter test_library

test_adapter: test_adapter.cc ../libpEpAdapter.a

test_library: test_library.cc ../libpEpAdapter.a

clean:
	rm -vf test_adapter
	rm -rvf test_adapter.dSYM
	rm -vf test_library
	rm -rvf test_library.dSYM
	
