## Process this file with automake to produce Makefile.in

SUBDIRS = po \
	libseed \
	src \
	extensions \
	doc \
	modules \
	tests \
	examples

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

if PROFILE_MODULES
	cp modules/readline/*.c modules/readline/.libs/
	cp modules/example/*.c modules/example/.libs/
	cp modules/sqlite/*.c modules/sqlite/.libs/
	cp modules/canvas/*.c modules/canvas/.libs/
	cp modules/Multiprocessing/*.c modules/Multiprocessing/.libs/
endif

	cd libseed/.libs ; gcov *.c

if PROFILE_MODULES
	cd modules/readline/.libs ; gcov *.c
	cd modules/example/.libs ; gcov *.c
	cd modules/sqlite/.libs ; gcov *.c
	cd modules/canvas/.libs ; gcov *.c
	cd modules/Multiprocessing/.libs ; gcov *.c
endif

profile-gui: profile
if PROFILE_MODULES
	$mods_prof = --directory modules/readline/.libs --directory modules/example/.libs --directory modules/sqlite/.libs
endif

	lcov --directory libseed/.libs $(mods_prof) --capture --output-file seed.info
	cp seed.info /tmp/
	cd /tmp ; genhtml seed.info ; gnome-www-browser index.html
