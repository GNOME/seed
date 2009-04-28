if BUILD_MULTIPROCESSING_MODULE

EXTRA_DIST = fork.js bi.js

seedlibdir = ${exec_prefix}/lib/seed

seedlib_LTLIBRARIES = \
	libmultiprocessing.la

libmultiprocessing_la_SOURCES = \
	multi.c

libmultiprocessing_la_CFLAGS = \
        $(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
        $(SEED_DEBUG_CFLAGS) \
        $(SEED_PROFILE_CFLAGS)

libmultiprocessing_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif


