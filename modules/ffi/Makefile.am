if BUILD_FFI_MODULE

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

EXTRA_DIST = \
	test-sin.js \
	test-strcmp.js \
	test-strdup.js \
	test-xlib.js

seedlib_LTLIBRARIES = \
	libseed_ffi.la

libseed_ffi_la_SOURCES = \
	seed-ffi.c

libseed_ffi_la_CFLAGS = \
	-I@top_srcdir@/libseed/ \
	-Wall \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS) \
	$(FFI_CFLAGS)

libseed_ffi_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(FFI_LDFLAGS)

libseed_ffi_la_LIBADD = \
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	$(SEED_PROFILE_LIBS)

endif




-include $(top_srcdir)/git.mk
