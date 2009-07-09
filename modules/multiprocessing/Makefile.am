if BUILD_MULTIPROCESSING_MODULE

EXTRA_DIST = fork.js bi.js

seedlibdir = ${libdir}/seed

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
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif


