lib_LTLIBRARIES = \
	libexample.la

libexample_la_SOURCES = \
	example.c

libexample_la_LDFLAGS = \
	`pkg-config --libs seed`

AM_CPPFLAGS = \
	`pkg-config --cflags seed` -g

libdir = ${exec_prefix}/lib/seed

