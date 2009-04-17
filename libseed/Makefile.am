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
	seed-importer.h 

libseed_la_LIBADD = $(LIBSEED_LIBS) $(LIBFFI_LIBS)
libseed_la_CPPFLAGS = -lgmodule-2.0 $(LIBSEED_CFLAGS) $(SEED_OSX_CFLAGS) $(FFI_CFLAGS) -Wall
libseed_la_LDFLAGS = -no-undefined $(SEED_PROFILE_LDFLAGS) $(SEED_OSX_LDFLAGS) $(FFI_LIBS)

seedheaders_HEADERS = seed.h

seedheadersdir = $(pkgincludedir)

LIBSEED_CFLAGS += $(SEED_DEBUG_CFLAGS) $(SEED_PROFILE_CFLAGS)
