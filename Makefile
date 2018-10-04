# this file is under GNU GPL 3.0, see LICENSE.txt

include Makefile.conf
-include local.conf

TARGET?=libpEpAdapter.a
PEPENGINE_IN?=$(HOME)

CXXFLAGS += -I$(HOME)/include -std=c++14

SOURCE=$(wildcard *.cc)
OBJECTS=$(subst .cc,.o,$(SOURCE))

all: $(TARGET)

%.o: %.cc %.hh
	$(CXX) $(CXXFLAGS) -c $<

$(TARGET): $(OBJECTS)
	ar -rc $@ $^

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJECTS)
