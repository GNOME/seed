if BUILD_CAIRO_MODULE

EXTRA_DIST = seed-cairo-enums.h \
             seed-cairo.h \
             seed-cairo-image-surface.h \
             seed-cairo-matrix.h \
             seed-cairo-pattern.h \
             seed-cairo-pdf-surface.h \
             seed-cairo-surface.h

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

seedlib_LTLIBRARIES = \
	libseed_cairo.la

libseed_cairo_la_SOURCES = \
	seed-cairo.c \
	seed-cairo-surface.c \
	seed-cairo-image-surface.c \
	seed-cairo-pdf-surface.c \
	seed-cairo-matrix.c \
	seed-cairo-pattern.c \
	seed-cairo-enums.c 

libseed_cairo_la_CFLAGS = \
	-Wall \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	$(CAIRO_CFLAGS) \
	$(GDK_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_cairo_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(GDK_LDFLAGS) \
	$(CAIRO_LDFLAGS)

libseed_cairo_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la
	$(SEED_PROFILE_LIBS)

endif

