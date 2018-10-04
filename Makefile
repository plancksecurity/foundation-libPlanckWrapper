# this file is under GNU GPL 3.0, see LICENSE.txt

include Makefile.conf
-include local.conf

CXXFLAGS += -I$(HOME)/include -std=c++14 -O0 -g

SOURCE=$(wildcard *.cc)
HEADERS=$(wildcard *.hh)
OBJECTS=$(subst .cc,.o,$(SOURCE))
WITHOUT_TESTS=$(patsubst test%.o,,$(OBJECTS))

all: $(TARGET)

%.o: %.cc %.hh
	$(CXX) $(CXXFLAGS) -c $<

$(TARGET): $(WITHOUT_TESTS)
	ar -rc $@ $^

.PHONY: clean test install

clean:
	rm -f $(TARGET) $(OBJECTS) *.a test_adapter

test: test_adapter
	./test_adapter

test_adapter: test_adapter.o $(TARGET)
	$(CXX) -o $@ -L$(PEP)/lib -lpEpEngine -L. -lpEpAdapter $<

install:
	-mkdir -p $(PEP)/include
	cp $(HEADERS) $(PEP)/include

