# this file is under GNU GPL 3.0, see LICENSE.txt
# see LICENSE.txt

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

.PHONY: clean distclean test install uninstall

clean:
	rm -f $(TARGET) $(OBJECTS) *.a test_adapter lib

distclean: clean
	rm -Rf .gnupg .pEp_management*

test: test_adapter test_library
ifeq ($(HOME),$(PREFIX))
	-ln -fs $$HOME/lib
endif
	HOME=$$(pwd) ./test_adapter

test_adapter: test_adapter.o $(TARGET)
	$(CXX) -o $@ -L$(PEP)/lib -lpEpEngine -L. -lpEpAdapter $<

test_library: test_library.o $(TARGET)
	$(CXX) -o $@ -L$(PEP)/lib -lpEpEngine -L. -lpEpAdapter $<

install: $(TARGET)
	-mkdir -p $(PEP)/include
	cp $(HEADERS) $(PEP)/include
	cp $(TARGET) $(PEP)/lib

uninstall:
	cd $(PEP)/include && rm -f $(HEADERS)
	cd $(PEP)/lib && rm -f $(TARGET)
