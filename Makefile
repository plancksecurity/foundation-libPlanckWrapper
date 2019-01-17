# Copyright 2018, pEp Foundation
# This file is part of lib pEp Adapter
# This file may be used under the terms of the GNU General Public License version 3
# see LICENSE.txt

include Makefile.conf

ifneq ($(wildcard local.conf),)
    $(info ================================================)
    $(info Overrides in \`local.conf\` are used.)
    $(info ================================================)
endif

ifdef BUILD_CONFIG
    $(info ================================================)
    $(info Overrides in \`$(BUILD_CONFIG)\` are used.)
    $(info ================================================)
endif

SOURCE=$(wildcard *.cc)
HEADERS=$(wildcard *.hh *.hxx)
OBJECTS=$(subst .cc,.o,$(SOURCE))
WITHOUT_TESTS=$(patsubst test%.o,,$(OBJECTS))
TARGET=libpEpAdapter.a

EXTRA_LIB_PATHS=.:
ifdef ENGINE_LIB
	EXTRA_LIB_PATHS:=$(EXTRA_LIB_PATHS)$(patsubst -L%,%,$(ENGINE_LIB)):
endif

# Remove trailing ':'
EXTRA_LIB_PATHS:=$(EXTRA_LIB_PATHS::=)

ifeq ($(BUILD_FOR),Darwin)
    LIBPATH=DYLD_LIBRARY_PATH
else
    LIBPATH=LD_LIBRARY_PATH
endif

TEST_CMD_PFX=$(LIBPATH)=$(EXTRA_LIB_PATHS)


.PHONY: all
all: $(TARGET)

# Rule copied from make's built-in rules
%.o: %.cc %.hh
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

$(TARGET): $(WITHOUT_TESTS)
	ar -rc $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS) *.a test_adapter test_library lib
	rm -rf test_adapter.dSYM
	rm -rf test_library.dSYM
	rm -rf .gnupg/
	rm -f .pEp_management.db*

.PHONY: distclean
distclean: clean
	rm -Rf .gnupg .pEp_management*

# $$(pwd) will return the incorrect directory, if make is run with `make -C ...`.
.PHONY: test
test: test_adapter test_library
	 $(TEST_CMD_PFX) HOME=$$(pwd) ./test_adapter

test_%: test_%.o $(TARGET)

.PHONY: install
install: $(TARGET)
	-mkdir -p $(PREFIX)/include/pEp
	cp $(HEADERS) $(PREFIX)/include/pEp/
	cp $(TARGET) $(PREFIX)/lib/

.PHONY: uninstall
uninstall:
	cd $(PREFIX)/include/pEp && rm -f $(HEADERS)
	cd $(PREFIX)/lib && rm -f $(TARGET)
