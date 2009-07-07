## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po \
	libseed \
	src \
	extensions \
	modules \
	tests \
	examples \
	doc

seeddocdir = ${prefix}/share/doc/seed
seeddoc_DATA = \
	RELEASE \
	README \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL

pkgconfig_DATA = seed.pc 
pkgconfigdir = $(libdir)/pkgconfig

man_MANS = doc/seed.1

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

if PROFILE_MODULES
	cp modules/readline/*.c modules/readline/.libs/
	cp modules/example/*.c modules/example/.libs/
	cp modules/sqlite/*.c modules/sqlite/.libs/
	cp modules/canvas/*.c modules/canvas/.libs/
	cp modules/Multiprocessing/*.c modules/Multiprocessing/.libs/
endif

	cd libseed/.libs ; gcov *.c

if PROFILE_MODULES
	cd modules/cairo/.libs ; gcov *.c
	cd modules/canvas/.libs ; gcov *.c
	cd modules/dbus/.libs ; gcov *.c
	cd modules/example/.libs ; gcov *.c
	cd modules/gettext/.libs ; gcov *.c
	cd modules/gtkbuilder/.libs ; gcov *.c
	cd modules/libxml/.libs ; gcov *.c
	cd modules/os/.libs ; gcov *.c
	cd modules/readline/.libs ; gcov *.c
	cd modules/sandbox/.libs ; gcov *.c
	cd modules/sqlite/.libs ; gcov *.c
endif

profile-gui: profile

if PROFILE_MODULES
	lcov --directory libseed/.libs --directory modules/cairo/.libs --directory modules/canvas/.libs --directory modules/dbus/.libs --directory modules/example/.libs --directory modules/gettext/.libs --directory modules/gtkbuilder/.libs --directory modules/libxml/.libs --directory modules/os/.libs --directory modules/readline/.libs --directory modules/sandbox/.libs --directory modules/sqlite/.libs --capture --output-file seed.info
else
	lcov --directory libseed/.libs --capture --output-file seed.info
endif

	cp seed.info /tmp/
	cd /tmp ; genhtml seed.info ; gnome-open index.html
