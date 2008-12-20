## Process this file with automake to produce Makefile.in

SUBDIRS = po \
	libseed \
	src \
	extensions \
	doc \
	examples \
	modules \
	tests

seeddocdir = ${prefix}/share/doc/seed
seeddoc_DATA = \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS

pkgconfig_DATA = seed.pc 

pkgconfigdir = $(libdir)/pkgconfig

EXTRA_DIST = $(seeddoc_DATA) \
	$(pkgconfig_DATA)

DISTCHEKC_CONFIGURE_FLAGS=--enable-gtk-doc

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

test:
	cd tests ; ./run-tests.py

profile-reset:
	find -name '*.gcda' -delete

profile: profile-reset test
	cp libseed/*.c libseed/.libs/
	cd libseed/.libs ; gcov *.c

profile-gui: profile
	lcov --directory libseed/.libs --capture --output-file seed.info
	cp seed.info /tmp/
	cd /tmp ; genhtml seed.info ; gnome-open index.html
