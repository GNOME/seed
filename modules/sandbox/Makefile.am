seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libsandbox.la

libsandbox_la_SOURCES = \
	seed-sandbox.c


AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libsandbox_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)




