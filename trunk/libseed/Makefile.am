
lib_LTLIBRARIES = \
	libseed.la

libseed_la_SOURCES = \
	seed-closure.c \
	seed-closure.h \
	seed-builtins.c\
	seed-builtins.h \
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
	seed-types.h

libseed_la_LIBADD = \
	$(LIBSEED_LIBS)

AM_CPPFLAGS = \
	$(LIBSEED_CFLAGS)

seedheaders_HEADERS = \
	seed.h

seedheadersdir = \
	$(pkgincludedir)

EXTRA_DIST = \
	$(seedheaders_HEADERS)

## File created by the gnome-build tools

