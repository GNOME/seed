if BUILD_MPFR_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libmpfr.la

libmpfr_la_SOURCES = \
	mpfr.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) 
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libmpfr_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

AM_LDFLAGS = -lmpfr -lgmp

endif



