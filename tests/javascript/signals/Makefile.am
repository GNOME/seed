
TESTS_ENVIRONMENT = PATH=$(top_builddir)/src:$$PATH

workingtests = \
	signal-connect.js \
	signal-disconnect.js \
	signal-exception.js \
	signal-expects.js \
	signal-invalid.js \
	signal.js \
	signal-nofunc.js \
	signal-userdata.js

failingtests = \
	$(NULL)

otherfiles = testsuite.js

installingtests_DATA = $(workingtests) $(otherfiles)
installingtestsdir = $(datadir)/seed@SEED_GTK_VERSION@-tests/javascript/signals/

EXTRA_DIST = \
	$(workingtests) \
	$(failingtests) \
	$(otherfiles) \
	$(NULL)

TESTS = \
	$(workingtests) \
	$(failingtests) \
	$(NULL)

XFAIL_TESTS = \
	$(failingtests) \
	$(NULL)

-include $(top_srcdir)/git.mk
