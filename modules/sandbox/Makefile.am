seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libseed_sandbox.la

libseed_sandbox_la_SOURCES = \
	seed-sandbox.c


AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_sandbox_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)




