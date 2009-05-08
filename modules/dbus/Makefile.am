if BUILD_DBUS_MODULE

seedlibdir = ${exec_prefix}/lib/seed

seedlib_LTLIBRARIES = \
	libdbus.la

libdbus_la_SOURCES = \
	module.c \
	util/dbus.c \
	util/dbus-proxy.c \
	util/dbus-signals.c


libdbus_la_CFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)


libdbus_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS) \
	$(DBUS_LDFLAGS)

endif



