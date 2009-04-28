if BUILD_SEED_EXAMPLES
if BUILD_TURTLE_EXAMPLE

bin_PROGRAMS = seed_turtle

seed_turtle_SOURCES = \
	turtle.c

seed_turtle_CFLAGS = \
	-Wall \
	-I@top_srcdir@/libseed \
	$(GTK_CFLAGS) \
	$(GOBJECT_INTROSPECTION_CFLAGS)

seed_turtle_LDFLAGS = \
	$(GTK_LIBS)

seed_turtle_LDADD = \
	../../libseed/libseed.la

endif
endif
