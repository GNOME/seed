noinst_PROGRAMS = test

test_SOURCES =					\
	main.c						\
	basic.c						\
	api-types.c					\
	api-closure.c				\
	api-js-signal-from-c.c

EXTRA_DIST =					\
	test-common.h
	
test_CFLAGS = \
	-Wall \
	$(GOBJECT_INTROSPECTION_CFLAGS)

test_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LIBS)

test_LDADD = \
	../../libseed/libseed.la

