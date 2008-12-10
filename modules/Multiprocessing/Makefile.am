lib_LTLIBRARIES = \
	libmultiprocessing.la

libmultiprocessing_la_SOURCES = \
	multi.c

libmultiprocessing_la_LDFLAGS = \
	`pkg-config --libs seed`

AM_CPPFLAGS = \
	-I../../libseed/ \
	`pkg-config --cflags seed` \
	`pkg-config --cflags glib-2.0` -g \
	`pkg-config --cflags gobject-introspection-1.0`

EXTRA_DIST = fork.js bi.js

libdir = ${exec_prefix}/lib/seed

