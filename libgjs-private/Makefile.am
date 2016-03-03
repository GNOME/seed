-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =

lib_LTLIBRARIES = \
	libseed_gjsprivate.la

libgjs_private_source_files = \
	gjs-gdbus-wrapper.c    \
	gjs-gdbus-wrapper.h      \
	gjs-util.c     \
	gjs-util.h       \
	gjs-gtk-util.h   \
	gjs-gtk-util.c   \
	$(NULL)

libseed_gjsprivate_la_SOURCES = $(libgjs_private_source_files)

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(GTK_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_gjsprivate_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS)

libseed_gjsprivate_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(GTK_LIBS) \
	$(SEED_PROFILE_LIBS)

INTROSPECTION_SCANNER_ARGS = \
	--add-include-path=@top_srcdir@ --add-include-path=$(GTK_CFLAGS) --warn-all
INTROSPECTION_COMPILER_ARGS = \
	--includedir=@top_srcdir@

GjsPrivate-1.0.gir: libseed_gjsprivate.la
GjsPrivate_1_0_gir_INCLUDES = GObject-2.0 Gio-2.0
GjsPrivate_1_0_gir_CFLAGS = $(INCLUDES)
GjsPrivate_1_0_gir_LIBS = libseed_gjsprivate.la
GjsPrivate_1_0_gir_FILES = $(libgjs_private_source_files)
INTROSPECTION_GIRS += GjsPrivate-1.0.gir

GjsPrivate_1_0_gir_SCANNERFLAGS = --identifier-prefix=Gjs --symbol-prefix=gjs_ --warn-all
girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(pkglibdir)@SEED_GTK_VERSION@/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = $(gir_DATA) $(typelib_DATA)

