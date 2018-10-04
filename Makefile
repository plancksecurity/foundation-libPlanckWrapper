# this file is under GNU GPL 3.0, see LICENSE.txt

include Makefile.conf
-include local.conf

TARGET?=libpEpAdapter.a
PEPENGINE_IN?=$(HOME)

CXXFLAGS += -I$(HOME)/include -std=c++14

SOURCE=$(wildcard *.cc)
OBJECTS=$(subst .cc,.o,$(SOURCE))
WITHOUT_TESTS=$(patsubst test%.o,,$(OBJECTS))

all: $(TARGET)

%.o: %.cc %.hh
	$(CXX) $(CXXFLAGS) -c $<

$(TARGET): $(WITHOUT_TESTS)
	ar -rc $@ $^

.PHONY: clean test

clean:
	rm -f $(TARGET) $(OBJECTS) *.a test_adapter

test: test_adapter
	./test_adapter

test_adapter: $(OBJECTS)
	$(CXX) -o $@ -L$(HOME)/lib -lpEpEngine $(OBJECTS)

