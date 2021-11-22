# Copyright 2018, pEp Foundation
# This file is part of lib pEp Adapter
# This file may be used under the terms of the GNU General Public License version 3
# see LICENSE.txt

include ../Makefile.conf

SRC_CXX=$(wildcard *.cc)
SRC_C=$(wildcard *.c)
HEADERS=$(wildcard *.hh *.hxx *.h)
OBJ_CXX=$(subst .cc,.o,$(SRC_CXX))
OBJ_C=$(subst .c,.o,$(SRC_C))
OBJECTS=$(OBJ_CXX) $(OBJ_C)
DEPENDS=$(subst .cc,.d,$(SRC_CXX))
CFLAGS=-fPIC
CXXFLAGS+= -MMD -MP

ifneq ($(MAKECMDGOALS),clean)
    -include $(DEPENDS)
endif

.PHONY: all install uninstall clean
.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) -rc $@ $^

clean:
	rm -vf $(TARGET) $(OBJECTS) $(DEPENDS)
	rm -f *.d.*

install: $(TARGET)
	mkdir -p $(PREFIX)/include/pEp
	mkdir -p $(PREFIX)/lib
	cp -v $(HEADERS) $(PREFIX)/include/pEp/
	cp -v $(TARGET) $(PREFIX)/lib/

uninstall:
	cd $(PREFIX)/include/pEp && rm -vf $(HEADERS)
	cd $(PREFIX)/lib && rm -vf $(TARGET)
