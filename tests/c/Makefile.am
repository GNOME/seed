TESTS_ENVIRONMENT = \
	GI_TYPELIB_PATH=$(top_builddir)/libgjs-private:$$GI_TYPELIB_PATH  \
	LD_LIBRARY_PATH=$(top_builddir)/libgjs-private/.libs:$$LD_LIBRARY_PATH \
	SEED_MODULE_PATH=$(top_srcdir)/modules:$$SEED_MODULE_PATH \
	$(NULL)

noinst_PROGRAMS =
TESTS =

noinst_PROGRAMS += test test-module-whitelist
TESTS += test test-module-whitelist

test_SOURCES =					\
	main.c						\
	basic.c						\
	api-types.c					\
	api-closure.c				\
	api-js-signal-from-c.c			\
	$(NULL)

test_CFLAGS = \
	-Wall \
	$(GOBJECT_INTROSPECTION_CFLAGS)

test_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LIBS)

test_LDADD = \
	../../libseed/libseed@SEED_GTK_VERSION@.la

noinst_PROGRAMS += test-module-whitelist
TESTS += test-module-whitelist

test_module_whitelist_SOURCES = \
	test-module-whitelist.c \
	$(NULL)

test_module_whitelist_CFLAGS = \
	-Wall \
	$(GOBJECT_INTROSPECTION_CFLAGS)

test_module_whitelist_LDFLAGS = \
	$(GOBJECT_INTROSPECTION_LIBS)

test_module_whitelist_LDADD = \
	../../libseed/libseed@SEED_GTK_VERSION@.la

EXTRA_DIST =					\
	test-common.h

-include $(top_srcdir)/git.mk
