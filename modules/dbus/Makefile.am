if BUILD_DBUS_MODULE

SUBDIRS = util

EXTRA_DIST = dbus2js.py \
			 dbus.js \
			 dbus-values.h \
			 lang.js \
			 dbus-exports.h

moduledir = $(datadir)/seed@SEED_GTK_VERSION@
module_DATA = dbus.js lang.js

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

seedlib_LTLIBRARIES = \
	libseed_dbusnative.la

libseed_dbusnative_la_SOURCES = \
	module.c \
	util/dbus.c \
	util/dbus-proxy.c \
	util/dbus-signals.c \
	dbus-values.c  \
	dbus-exports.c 


libseed_dbusnative_la_CFLAGS = \
	-Wall \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)


libseed_dbusnative_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(DBUS_LDFLAGS) \
	-L/lib

libseed_dbusnative_la_LIBADD = \
	-ldbus-1 \
	-ldbus-glib-1 \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(SEED_PROFILE_LIBS)


endif



