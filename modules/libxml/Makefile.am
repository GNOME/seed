if BUILD_LIBXML_MODULE

EXTRA_DIST = xml.js

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	liblibxml.la

liblibxml_la_SOURCES = \
	libxml.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(LIBXML_CFLAGS) \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

liblibxml_la_LDFLAGS = \
	$(LIBXML_LDFLAGS) \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

extensionsdir=$(datadir)/seed/extensions
extensions_DATA=xml.js

endif



