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

.PHONY: test
.PHONY: test-report test-report-normal

test:
	@gtester -o=test-results.xml ./test

test-report-normal:
	@gtester -o=test-results.xml -k ./test \
		&& ( gtester-report test-results.xml > test-results.html ) \
		&& gnome-open ./test-results.html

test-report: test-report-normal

