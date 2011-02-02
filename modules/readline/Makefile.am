if BUILD_READLINE_MODULE

seedlibdir = ${libdir}/seed@SEED_GTK_VERSION@

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
	$(top_builddir)/libseed/libseed@SEED_GTK_VERSION@.la \
	-lreadline \
	$(SEED_PROFILE_LIBS) 

endif
