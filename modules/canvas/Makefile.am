lib_LTLIBRARIES = \
	libcanvas.la

libcanvas_la_SOURCES = \
	seed-canvas.c

libsqlite_la_LDFLAGS = \
	`pkg-config --libs seed cairo`

EXTRA_DIST=example.js

AM_CPPFLAGS = \
	-I../../libseed/ \
	`pkg-config --cflags seed` \
	`pkg-config --cflags glib-2.0` -g \
	`pkg-config --cflags gobject-introspection-1.0` \
	`pkg-config --cflags cairo`

libdir = ${exec_prefix}/lib/seed

