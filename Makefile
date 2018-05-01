# Makefile
#
# Copyright (C) 2018 Kano Computing Ltd.
# License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
#
# Standardised project interface.


.PHONY: all release debug clean clean-release clean-debug


all: release debug

release:
	mkdir -p build/release && \
		cd build/release && \
		cmake -D BUILD_TYPE=Release ../ && \
		make

debug:
	mkdir -p build/debug && \
		cd build/debug && \
		cmake -D BUILD_TYPE=Debug ../ && \
		make

clean: clean-release clean-debug

clean-release:
	-rm -rf build/release
	-rm -f bin/ovlsnap
	-rm -f bin/ovlsnap-init

clean-debug:
	-rm -rf build/debug
	-rm -f bin/ovlsnap-dbg
	-rm -f bin/ovlsnap-init-dbg

