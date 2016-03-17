seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@/gjs

seedlib_LTLIBRARIES = \
	libseed__gi.la

libseed__gi_la_SOURCES = \
	seed-gi.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed__gi_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS)

libseed__gi_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(SEED_PROFILE_LIBS)

