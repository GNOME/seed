if BUILD_MPFR_MODULE

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

seedlib_LTLIBRARIES = \
	libseed_mpfr.la

libseed_mpfr_la_SOURCES = \
	seed-mpfr.c \
	seed-mpfr-trig.c \
	seed-mpfr-arithmetic.c \
	seed-mpfr-cmp.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_mpfr_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS)

libseed_mpfr_la_LIBADD = -lmpfr -lgmp \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(SEED_PROFILE_LIBS)

EXTRA_DIST = seed-mpfr.h

endif




-include $(top_srcdir)/git.mk
