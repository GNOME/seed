if BUILD_GTKBUILDER_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libseed_gtkbuilder.la

libseed_gtkbuilder_la_SOURCES = \
	seed-gtkbuilder.c


AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(GTK_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_gtkbuilder_la_LDFLAGS = \
	-module -avoid-version \
	$(GTK_LDFLAGS) \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

endif



