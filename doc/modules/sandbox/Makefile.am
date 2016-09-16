EXTRA_DIST = \
	sandbox.js sandbox.xml

sandbox-funcs.xml: $(top_builddir)/src/seed $(top_srcdir)/doc/modules/sandbox/sandbox.js 
	$(top_builddir)/src/seed $(top_srcdir)/doc/modules/make-functions.js $(top_srcdir)/doc/modules/sandbox/sandbox.js > $(top_builddir)/doc/modules/sandbox/sandbox-funcs.xml
CLEANFILES=sandbox-funcs.xml

sandboxdocsdir = \
	$(docdir)/modules/sandbox/html
sandboxdocs_DATA = sandbox-funcs.xml

-include $(top_srcdir)/git.mk
