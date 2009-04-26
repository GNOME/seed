## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(SEED_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 $(SEED_DEBUG_CFLAGS)\
	 $(SEED_PROFILE_CFLAGS)

bin_PROGRAMS = seed

seed_SOURCES = \
	main.c

seed_LDFLAGS = \
	-L../libseed/.libs -lseed $(SEED_PROFILE_LDFLAGS)

seed_LDADD = \
	$(SEED_LIBS) \
	$(LIBFFI_LDFLAGS) \
	-lseed
