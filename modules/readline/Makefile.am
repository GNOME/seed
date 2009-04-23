if BUILD_READLINE_MODULE

lib_LTLIBRARIES = \
	libreadline.la

libreadline_la_SOURCES = \
	seed-readline.c

libreadline_la_LDFLAGS = \
	-lreadline \
	$(SEED_LDFLAGS)

AM_CPPFLAGS = \
	-I../../libseed/ \
	`pkg-config --cflags seed` \
	`pkg-config --cflags glib-2.0` -g \
	`pkg-config --cflags gobject-introspection-1.0` \
	`pkg-config --cflags libffi` \
	$(SEED_DEBUG_CFLAGS)

if PROFILE_MODULES
	AM_CPPFLAGS += $(SEED_PROFILE_CFLAGS)
	AM_LDFLAGS += $(SEED_PROFILE_LDFLAGS)
endif

endif

libdir = ${exec_prefix}/lib/seed
