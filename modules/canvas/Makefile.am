lib_LTLIBRARIES = \
	libcanvas.la

libcanvas_la_SOURCES = \
	seed-canvas.c

libcanvas_la_LDFLAGS = \
	`pkg-config --libs seed cairo`

EXTRA_DIST=test1.js test2.js test3.js test4.js test5.js test6.js test7.js test8.js test9.js test10.js test11.js test12.js test13.js test14.js test15.js test16.js

AM_CPPFLAGS = \
	-I../../libseed/ \
	`pkg-config --cflags seed` \
	`pkg-config --cflags glib-2.0` -g \
	`pkg-config --cflags gobject-introspection-1.0` \
	`pkg-config --cflags cairo`

libdir = ${exec_prefix}/lib/seed

