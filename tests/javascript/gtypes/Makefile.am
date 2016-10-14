
TESTS_ENVIRONMENT = PATH=$(top_builddir)/src:$$PATH

workingtests = \
	gtype-class-init-exception.js \
	gtype-typerror.js \
	gtype.js \
	gtype-gtype.js \
	gtype-signal-args.js \
	gtype-signal-order-bug663289.js \
	gtype-property-construct.js \
	gtype-property-nice.js \
	gtype-self.js \
	gtype-extraprop.js \
	gtype-gtype-class-init.js \
	gtype-signal.js

failingtests = \
	$(NULL)

otherfiles = testsuite.js

installingtests_DATA = $(workingtests) $(otherfiles)
installingtestsdir = $(datadir)/seed@SEED_GTK_VERSION@-tests/javascript/gtypes/

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
