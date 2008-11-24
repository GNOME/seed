## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = po \
	libseed\
	src\
	extensions\
	gir \
	doc\
	examples\
        modules \
	tests
	

seeddocdir = ${prefix}/share/doc/seed
seeddoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


pkgconfig_DATA = seed.pc 

pkgconfigdir = $(libdir)/pkgconfig
	

EXTRA_DIST = $(seeddoc_DATA) \
	$(pkgconfig_DATA)


# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

test:
	cd tests ; ./run-tests.py

