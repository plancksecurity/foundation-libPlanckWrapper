# Copyright 2018, pEp Foundation
# This file is part of lib pEp Adapter
# This file may be used under the terms of the GNU General Public License version 3
# see LICENSE.txt

.PHONY: src test install uninstall clean

all: src

src:
	$(MAKE) -C src

test-fw: src
	$(MAKE) -C test/fw_dist_test

test: src test-fw
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean

install:
	$(MAKE) -C src install

uninstall:
	$(MAKE) -C src uninstall