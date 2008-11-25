lib_LTLIBRARIES = \
	libsqlite.la

libsqlite_la_SOURCES = \
	seed-sqlite.c

libsqlite_la_LDFLAGS = \
	`pkg-config --libs seed sqlite3`

AM_CPPFLAGS = \
	`pkg-config --cflags seed sqlite3` -g

libdir = ${exec_prefix}/lib/seed

