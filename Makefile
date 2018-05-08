# Makefile
#
# Copyright (C) 2018 Kano Computing Ltd.
# License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
#
# Standardised project interface.


all: debug

debug:
	mkdir -p build/debug && \
		cd build/debug && \
		cmake ../ && \
		make --jobs

release: debug
	# apt-get install -y binutils
	strip --strip-all bin/ovlsnap
	strip --strip-all bin/ovlsnap-init

dev:
	mkdir -p build/dev && \
		cd build/dev && \
		cmake -D BUILD_TYPE=Development ../ && \
		make --jobs

clean: clean-bin clean-debug clean-dev clean-debian

clean-bin:
	-rm -f bin/ovlsnap
	-rm -f bin/ovlsnap-init

clean-debug:
	-rm -rf build/debug

clean-dev:
	-rm -rf build/dev

clean-debian:
	-rm -rf debian/overlay-snapshots
	-rm -rf debian/overlay-snapshots-dbg
	-rm -rf debian/files
	-rm -rf debian/.debhelper
	-rm -rf debian/debhelper-build-stamp
	-rm -rf debian/*.log
	-rm -rf debian/*.substvars


.PHONY: all debug release dev
.PHONY: clean clean-bin clean-debug clean-dev clean-debian
