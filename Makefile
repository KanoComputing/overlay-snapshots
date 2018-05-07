# Makefile
#
# Copyright (C) 2018 Kano Computing Ltd.
# License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
#
# Standardised project interface.


.PHONY: all debug clean clean-debug clean-debian


all: debug

debug:
	mkdir -p build/debug && \
		cd build/debug && \
		cmake ../ && \
		make --jobs

clean: clean-debug clean-debian

clean-debug:
	-rm -rf build/debug
	-rm -f bin/ovlsnap
	-rm -f bin/ovlsnap-init

clean-debian:
	-rm -rf debian/overlay-snapshots
	-rm -rf debian/overlay-snapshots-dbg
	-rm -rf debian/files
	-rm -rf debian/debhelper-build-stamp
	-rm -rf debian/*.log
	-rm -rf debian/*.substvars
