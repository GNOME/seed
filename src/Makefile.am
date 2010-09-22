## Process this file with automake to produce Makefile.in

bin_PROGRAMS = seed

seed_SOURCES = \
	main.c \
	args.c

seed_CFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-Wall \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS) \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(WEBKIT_CFLAGS)

seed_LDFLAGS = \
	$(SEED_PROFILE_LIBS) \
	$(LIBFFI_LDFLAGS) \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(WEBKIT_LDFLAGS) \
	$(GTHREAD_LIBS)

seed_LDADD = \
	../libseed/libseed.la
