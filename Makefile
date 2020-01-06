# Copyright 2018, pEp Foundation
# This file is part of lib pEp Adapter
# This file may be used under the terms of the GNU General Public License version 3
# see LICENSE.txt

include Makefile.conf

ifneq ($(wildcard local.conf),)
    $(info ================================================)
    $(info Overrides in `local.conf` are used.)
    $(info ================================================)
endif

ifdef BUILD_CONFIG
    $(info ================================================)
    $(info Overrides in `$(BUILD_CONFIG)` are used.)
    $(info ================================================)
endif

.PHONY: all, clean, distclean, test, install, uninstall

SOURCE=$(wildcard *.cc)
HEADERS=$(wildcard *.hh *.hxx)
OBJECTS=$(subst .cc,.o,$(SOURCE))
TARGET=libpEpAdapter.a

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) -rc $@ $^

clean:
	rm -vf $(TARGET) $(OBJECTS)
#	rm -rvf .gnupg/
# rm -vf .pEp_management.db*

# distclean: clean
# 	rm -Rvf .gnupg .pEp_management*

install: $(TARGET)
	mkdir -p $(PREFIX)/include/pEp
	mkdir -p $(PREFIX)/lib
	cp -v $(HEADERS) $(PREFIX)/include/pEp/
	cp -v $(TARGET) $(PREFIX)/lib/

uninstall:
	cd $(PREFIX)/include/pEp && rm -vf $(HEADERS)
	cd $(PREFIX)/lib && rm -vf $(TARGET)
