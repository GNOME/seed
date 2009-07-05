if BUILD_MPFR_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libseed_mpfr.la

libseed_mpfr_la_SOURCES = \
	seed-mpfr.c
	seed-mpfr-trig.c
	seed-mpfr-arithmetic.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) 
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_mpfr_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

libseed_mpfr_la_LIBADD = -lmpfr -lgmp

endif



