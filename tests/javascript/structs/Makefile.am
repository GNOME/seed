
TESTS_ENVIRONMENT = PATH=$(top_builddir)/src:$$PATH

workingtests = \
	struct-functions.js \
	struct-set-member.js \
	struct-constructor.js \
	struct-nested-set.js \
	struct-null.js \
	struct-union-enumerate.js \
	struct-enumerate.js \
	struct-offsets.js

failingtests = \
	$(NULL)

otherfiles = testsuite.js

installingtests_DATA = $(workingtests) $(otherfiles)
installingtestsdir = $(datadir)/seed@SEED_GTK_VERSION@-tests/javascript/stucts/

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
