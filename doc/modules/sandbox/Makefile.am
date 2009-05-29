EXTRA_DIST = \
	sandbox.js sandbox.xml

sandbox-funcs.xml: $(top_builddir)/src/seed sandbox.js 
	$(top_builddir)/src/seed $(top_srcdir)/doc/modules/make-functions.js sandbox.js > $(top_builddir)/doc/modules/sandbox/sandbox-funcs.xml
CLEANFILES=sandbox-funcs.xml

sandboxdocsdir = \
	$(datadir)/doc/seed/modules/sandbox/html
sandboxdocs_DATA = sandbox-funcs.xml


