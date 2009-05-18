if BUILD_CAIRO_MODULE

seedlibdir = ${libdir}/seed
EXTRA_DIST=run-tests.js

seedlib_LTLIBRARIES = \
	libcairo.la

libcairo_la_SOURCES = \
	seed-cairo.c \
	seed-cairo-surface.c \
	seed-cairo-image-surface.c \
	seed-cairo-matrix.c \
	seed-cairo-pattern.c \
	seed-cairo-enums.c 

libcairo_la_CFLAGS = \
	-Wall \
        $(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	$(CAIRO_CFLAGS) \
	$(GDK_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libcairo_la_LDFLAGS = \
        $(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(GDK_LDFLAGS) \
	$(CAIRO_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif

