if BUILD_LIBXML_MODULE

EXTRA_DIST = xml.js

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libseed_libxml.la

libseed_libxml_la_SOURCES = \
	seed-libxml.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(LIBXML_CFLAGS) \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_libxml_la_LDFLAGS = \
	-module -avoid-version \
	$(LIBXML_LDFLAGS) \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

extensionsdir=$(datadir)/seed/extensions
extensions_DATA=xml.js

endif



