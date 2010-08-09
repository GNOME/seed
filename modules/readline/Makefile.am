if BUILD_READLINE_MODULE

seedlibdir = ${libdir}/seed

seedlib_LTLIBRARIES = \
	libseed_readline.la

libseed_readline_la_SOURCES = \
	seed-readline.c

libseed_readline_la_CFLAGS = \
	-I@top_srcdir@/libseed/ \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	$(FFI_CFLAGS) \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libseed_readline_la_LDFLAGS = \
	-module -avoid-version \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	$(FFI_LDFLAGS)

libseed_readline_la_LIBADD = \
	$(top_builddir)/libseed/libseed.la \
	-lreadline \
	$(SEED_PROFILE_LIBS) 

endif
