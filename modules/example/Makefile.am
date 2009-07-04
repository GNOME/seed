if BUILD_EXAMPLE_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libexample.la

libexample_la_SOURCES = \
	seed-example.c


AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libexample_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif



