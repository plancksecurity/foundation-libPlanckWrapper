# Copyright 2018, pEp Foundation
# This file is part of lib pEp Adapter
# This file may be used under the terms of the GNU General Public License version 3
# see LICENSE.txt

include Makefile.conf

TARGET=libpEpAdapter.a

.PHONY: install uninstall clean

SOURCE=$(wildcard *.cc)
HEADERS=$(wildcard *.hh *.hxx)
OBJECTS=$(subst .cc,.o,$(SOURCE))
DEPENDS=$(subst .cc,.d,$(SOURCE))
CXXFLAGS+= -MMD -MP

all: $(TARGET)

ifneq ($(MAKECMDGOALS),clean)
    -include $(DEPENDS)
endif

lib: $(TARGET)

all: lib

test: lib
	$(MAKE) -C test

$(TARGET): $(OBJECTS)
	$(AR) -rc $@ $^

clean:
	rm -vf $(TARGET) $(OBJECTS) $(DEPENDS)
	rm -f *.d.*
	$(MAKE) -C test clean

install: $(TARGET)
	mkdir -p $(PREFIX)/include/pEp
	mkdir -p $(PREFIX)/lib
	cp -v $(HEADERS) $(PREFIX)/include/pEp/
	cp -v $(TARGET) $(PREFIX)/lib/

uninstall:
	cd $(PREFIX)/include/pEp && rm -vf $(HEADERS)
	cd $(PREFIX)/lib && rm -vf $(TARGET)
