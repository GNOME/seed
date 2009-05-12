if BUILD_LIBXML_MODULE

seedlibdir = ${exec_prefix}/lib/seed

seedlib_LTLIBRARIES = \
	liblibxml.la

liblibxml_la_SOURCES = \
	libxml.c


AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS)
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

liblibxml_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif



