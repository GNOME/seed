if BUILD_READLINE_MODULE

seedlibdir = ${exec_prefix}/lib/seed

seedlib_LTLIBRARIES = \
	libreadline.la

libreadline_la_SOURCES = \
	seed-readline.c

libreadline_la_CFLAGS = \
	$(GOBJECT_INTROSPECTION_CFLAGS) \
	-I@top_srcdir@/libseed/ \
	$(SEED_DEBUG_CFLAGS) \
	$(SEED_PROFILE_CFLAGS)

libreadline_la_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LDFLAGS) \
	-lreadline \
	$(SEED_PROFILE_LIBS) 

endif
