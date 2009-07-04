if BUILD_CANVAS_MODULE

seedlibdir = ${libdir}/seed
EXTRA_DIST=run-tests.js

seedlib_LTLIBRARIES = \
	libseed_canvas.la

libseed_canvas_la_SOURCES = \
	seed-canvas.c

libseed_canvas_la_CFLAGS = \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	-I@top_srcdir@/modules/ \
	$(CAIRO_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_canvas_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(CAIRO_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif

