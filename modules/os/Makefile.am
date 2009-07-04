if BUILD_OS_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libos.la

libos_la_SOURCES = \
	seed-os.c

libos_la_CFLAGS = \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libos_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	-lutil \
	$(SEED_DEBUG_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif
