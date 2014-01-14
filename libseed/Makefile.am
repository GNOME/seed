lib_LTLIBRARIES = \
	libseed@SEED_GTK_VERSION@.la

libseed@SEED_GTK_VERSION@_la_SOURCES = \
	seed-closure.c \
	seed-api.c \
	seed-closure.h \
	seed-builtins.c \
	seed-builtins.h \
	seed-debug.h \
	seed-exceptions.c \
	seed-exceptions.h \
	seed-engine.c \
	seed-engine.h \
	seed-private.h \
	seed-signals.c \
	seed-signals.h \
	seed-structs.c \
	seed-gtype.c \
	seed-gtype.h \
	seed-structs.h \
	seed-types.c \
	seed-types.h \
	seed-importer.c \
	seed-importer.h \
	seed-path.h

libseed@SEED_GTK_VERSION@_la_CFLAGS = \
	-Wall \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(WEBKIT_CFLAGS) \
	$(SEED_OSX_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS) \
	$(FFI_CFLAGS) \
	-DGOBJECT_INTROSPECTION_VERSION=$(GOBJECT_INTROSPECTION_VERSION)

libseed@SEED_GTK_VERSION@_la_LDFLAGS = \
	$(FFI_LDFLAGS)

libseed@SEED_GTK_VERSION@_la_LIBADD = \
	$(GOBJECT_INTROSPECTION_LIBS) \
	$(WEBKIT_LIBS) \
	$(SEED_PROFILE_LIBS) \
	$(SEED_OSX_LIBS) \
	$(GIO_LIBS) \
	$(GMODULE_LIBS) \
	$(FFI_LIBS) $(LIBDL)

seedheaders_HEADERS = seed.h seed-debug.h seed-module.h

seedheadersdir = $(pkgincludedir)@SEED_GTK_VERSION@

