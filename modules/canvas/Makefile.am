if BUILD_CANVAS_MODULE

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@
EXTRA_DIST=run-tests.js

seedlib_LTLIBRARIES = \
	libseed_canvas.la

libseed_canvas_la_SOURCES = \
	seed-canvas.c

libseed_canvas_la_CFLAGS = \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	$(CAIRO_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_canvas_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(CAIRO_LDFLAGS)

libseed_canvas_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(CAIRO_LIBS) \
	$(SEED_PROFILE_LIBS)

endif


-include $(top_srcdir)/git.mk
