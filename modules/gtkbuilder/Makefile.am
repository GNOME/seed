if BUILD_GTKBUILDER_MODULE

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

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
	$(GOBJECT_INTROSPECTION_LDFLAGS)

libseed_gtkbuilder_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(SEED_PROFILE_LIBS)

endif



