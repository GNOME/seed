# Mostly stolen from Clutter.

NULL =

noinst_PROGRAMS = test

test_SOURCES =				\
	test-main.c				\
	test-tests.c			\
	$(NULL)
	
test_CFLAGS = -Wall				\
	 $(SEED_DEBUG_CFLAGS)		\
	 $(SEED_PROFILE_CFLAGS)		\
	 $(SEED_CFLAGS)

test_LDADD = $(SEED_LIBS) -lseed
test_LDFLAGS = -L../../libseed/.libs -lseed -lreadline $(SEED_PROFILE_LDFLAGS)

#test:
#	@gtester -o=test-results.xml ./test

