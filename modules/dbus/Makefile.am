if BUILD_DBUS_MODULE

seedlibdir = ${exec_prefix}/lib/seed

seedlib_LTLIBRARIES = \
	libdbusnative.la

libdbusnative_la_SOURCES = \
	module.c \
	util/dbus.c \
	util/dbus-proxy.c \
	util/dbus-signals.c


libdbusnative_la_CFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)


libdbusnative_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	-L/lib \
	-ldbus-1 \
	$(DBUS_LDFLAGS) \
	$(SEED_PROFILE_LIBS)


endif



