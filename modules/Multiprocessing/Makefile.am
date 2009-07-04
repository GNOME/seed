if BUILD_MULTIPROCESSING_MODULE

EXTRA_DIST = fork.js bi.js

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libmultiprocessing.la

libmultiprocessing_la_SOURCES = \
	seed-Multiprocessing.c

libmultiprocessing_la_CFLAGS = \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libmultiprocessing_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif


