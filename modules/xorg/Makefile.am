if BUILD_XORG_MODULE

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

seedlib_LTLIBRARIES = \
	libseed_xorg.la

libseed_xorg_la_SOURCES = \
	seed-xorg.c

libseed_xorg_la_CFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(LIBXSS_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_xorg_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(LIBXSS_LDLAGS) \
	$(SEED_DEBUG_LDFLAGS)

libseed_xorg_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(LIBXSS_LIBS) \
	$(SEED_PROFILE_LIBS)

endif

-include $(top_srcdir)/git.mk
