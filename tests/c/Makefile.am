TESTS_ENVIRONMENT = \
	GI_TYPELIB_PATH=$(top_builddir)/libgjs-private:$$GI_TYPELIB_PATH  \
	LD_LIBRARY_PATH=$(top_builddir)/libgjs-private/.libs:$$LD_LIBRARY_PATH \
	SEED_MODULE_PATH=$(top_srcdir)/modules:$$SEED_MODULE_PATH \
	$(NULL)

noinst_PROGRAMS = test

TESTS = test

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
	../../libseed/libseed@SEED_GTK_VERSION@.la

