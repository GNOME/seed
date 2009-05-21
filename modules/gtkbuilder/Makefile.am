if BUILD_GTKBUILDER_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libgtkbuilder.la

libgtkbuilder_la_SOURCES = \
	gtkbuilder.c


AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(GTK_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libgtkbuilder_la_LDFLAGS = \
	$(GTK_LDFLAGS) \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif



