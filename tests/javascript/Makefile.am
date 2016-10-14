
TESTS_ENVIRONMENT = PATH=$(top_builddir)/src:$$PATH

SUBDIRS = gtypes signals structs

workingtests = \
    argv.js \
    array-gtype.js \
    builtin-argument-length.js \
    check-syntax.js \
    closure-finalization.js \
    compare.js \
    constructor-args.js \
    constructor-prototype.js \
    enum.js \
    fork.js \
    function-info.js \
    gobject-scope.js \
    gvalue-argument.js \
    include.js \
    included.js \
    include-syntax.js \
    introspect.js \
    json-constructor.js \
    json.js \
    list-test.js \
    modules.js \
    modules-noasserts.js \
    native-closure-exception.js \
    native-closure.js \
    object-info.js \
    out-test.js \
    property-benchmark.js \
    property-glib-exception.js \
    quit.js \
    sqlite.js \
    syntax-test.js \
    syntax-test-noasserts.js \
    type-conversion.js


failingtests = \
	syntax-test-noasserts.js \
	modules-noasserts.js \
	included.js \
	gerror.js \
	gdk-event.js \
	$(NULL)

otherfiles = testsuite.js \
			 modules-noasserts.js.in \
			 syntax-test-noasserts.js.in \
    		 included.js.in \
			 COPYING \
			 $(NULL)

installingtests_DATA = $(workingtests) $(otherfiles)
installingtestsdir = $(datadir)/seed@SEED_GTK_VERSION@-tests/javascript/

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
