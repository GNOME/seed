noinst_PROGRAMS = test

test_SOURCES =					\
	main.c						\
	basic.c
	
test_CFLAGS = -Wall				\
	 $(SEED_CFLAGS)

test_LDADD = $(SEED_LIBS) -lseed
test_LDFLAGS = -L../../libseed/.libs -lseed -lreadline

