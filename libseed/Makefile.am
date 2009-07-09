lib_LTLIBRARIES = \
	libseed.la

libseed_la_SOURCES = \
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

libseed_la_CFLAGS = \
	-Wall \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(WEBKIT_CFLAGS) \
	$(SEED_OSX_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS) \
	$(FFI_CFLAGS)

libseed_la_LDFLAGS = \
	-no-undefined \
	$(GOBJECT_INTROSPECTION_LIBS) \
	$(WEBKIT_LIBS) \
	$(SEED_PROFILE_LIBS) \
	$(SEED_OSX_LIBS) \
	$(FFI_LDFLAGS)

seedheaders_HEADERS = seed.h seed-debug.h seed-module.h

seedheadersdir = $(pkgincludedir)

