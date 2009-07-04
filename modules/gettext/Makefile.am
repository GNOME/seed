if BUILD_GETTEXT_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libgettext.la

libgettext_la_SOURCES = \
	seed-gettext.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libgettext_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif



