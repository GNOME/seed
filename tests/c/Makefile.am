noinst_PROGRAMS = test

test_SOURCES =					\
	main.c						\
	basic.c						\
	api-types.c					\
	api-closure.c				\
	api-js-signal-from-c.c

EXTRA_DIST =					\
	test-common.h
	
test_CFLAGS = -Wall				\
	 $(SEED_CFLAGS)

test_LDADD = $(SEED_LIBS) -lseed
test_LDFLAGS = -L../../libseed/.libs -lseed

