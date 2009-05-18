if BUILD_SQLITE_MODULE

seedlibdir = ${libdir}/seed
EXTRA_DIST = example.js

seedlib_LTLIBRARIES = \
	libsqlite.la

libsqlite_la_SOURCES = \
	seed-sqlite.c

libsqlite_la_CFLAGS = \
        $(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SQLITE_CFLAGS) \
        $(SEED_DEBUG_CFLAGS) \
        $(SEED_PROFILE_CFLAGS)
	

libsqlite_la_LDFLAGS = \
        $(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SQLITE_LDFLAGS) \
        $(SEED_DEBUG_LDFLAGS) \
        $(SEED_PROFILE_LIBS)

endif
