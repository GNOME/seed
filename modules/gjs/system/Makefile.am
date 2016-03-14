seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@/gjs

seedlib_LTLIBRARIES = \
	libseed_system.la

libseed_system_la_SOURCES = \
	seed-system.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_system_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS)

libseed_system_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(SEED_PROFILE_LIBS)

