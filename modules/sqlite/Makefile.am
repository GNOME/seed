if BUILD_SQLITE_MODULE

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@
EXTRA_DIST = example.js

seedlib_LTLIBRARIES = \
	libseed_sqlite.la

libseed_sqlite_la_SOURCES = \
	seed-sqlite.c

libseed_sqlite_la_CFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SQLITE_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_sqlite_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SQLITE_LDFLAGS) \
	$(SEED_DEBUG_LDFLAGS)

libseed_sqlite_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(SQLITE_LIBS) \
	$(SEED_PROFILE_LIBS)

endif
