if BUILD_DYNAMICOBJECT_MODULE

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

seedlib_LTLIBRARIES = \
	libseed_DynamicObject.la

libseed_DynamicObject_la_SOURCES = \
	seed-DynamicObject.c

AM_CPPFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_DynamicObject_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS)

libseed_DynamicObject_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la

endif



