if BUILD_MULTIPROCESSING_MODULE

EXTRA_DIST = fork.js bi.js

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

seedlib_LTLIBRARIES = \
	libseed_multiprocessing.la

libseed_multiprocessing_la_SOURCES = \
	seed-multiprocessing.c

libseed_multiprocessing_la_CFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_multiprocessing_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS)

libseed_multiprocessing_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(SEED_PROFILE_LIBS)

endif


