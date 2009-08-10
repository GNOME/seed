if BUILD_FFI_MODULE

seedlibdir = ${libdir}/seed

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
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(SEED_PROFILE_LIBS) \
	$(FFI_LDFLAGS)

endif



