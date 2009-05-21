EXTRA_DIST = \
	gtkbuilder.js

gtkbuilder-funcs.xml: $(top_builddir)/src/seed gtkbuilder.js
	$(top_builddir)/src/seed ../make-functions.js gtkbuilder.js > gtkbuilder-funcs.xml
CLEANFILES=gtkbuilder-funcs.xml

gtkbuilderdocsdir = \
	$(datadir)/doc/seed/modules/gtkbuilder/html
gtkbuilderdocs_DATA = gtkbuilder-funcs.xml


