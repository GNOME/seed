if BUILD_CANVAS_MODULE

seedlibdir = ${exec_prefix}/lib/seed
EXTRA_DIST=run-tests.js

seedlib_LTLIBRARIES = \
	libcanvas.la

libcanvas_la_SOURCES = \
	seed-canvas.c

libcanvas_la_CFLAGS = \
        $(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	$(CAIRO_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libcanvas_la_LDFLAGS = \
        $(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(CAIRO_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif

