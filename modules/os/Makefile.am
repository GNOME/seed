if BUILD_OS_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libseed_os.la

libseed_os_la_SOURCES = \
	seed-os.c

libseed_os_la_CFLAGS = \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_os_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	-lutil \
	$(SEED_DEBUG_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif
