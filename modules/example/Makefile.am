lib_LTLIBRARIES = \
	libexample.la

libexample_la_SOURCES = \
	example.c

libexample_la_LDFLAGS = \
	`pkg-config --libs seed`

AM_CPPFLAGS = \
	-I../../libseed/ \
	`pkg-config --cflags seed` \
	`pkg-config --cflags glib-2.0` -g \
	`pkg-config --cflags gobject-introspection-1.0`

libdir = ${exec_prefix}/lib/seed

