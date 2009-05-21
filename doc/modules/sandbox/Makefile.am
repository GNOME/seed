EXTRA_DIST = \
	sandbox.js

sandbox-funcs.xml: $(top_builddir)/src/seed sandbox.js 
	$(top_builddir)/src/seed ../make-functions.js sandbox.js > sandbox-funcs.xml
CLEANFILES=sandbox-funcs.xml

sandboxdocsdir = \
	$(datadir)/doc/seed/modules/sandbox/html
sandboxdocs_DATA = sandbox-funcs.xml


