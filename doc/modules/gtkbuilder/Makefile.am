EXTRA_DIST = \
	gtkbuilder.js \
	example.ui \
	example.js \
	gtkbuilder.xml

gtkbuilder-funcs.xml: $(top_builddir)/src/seed $(top_srcdir)/doc/modules/gtkbuilder/gtkbuilder.js $(top_srcdir)/doc/modules/gtkbuilder/example.ui $(top_srcdir)/doc/modules/gtkbuilder/example.js
	$(top_builddir)/src/seed $(top_srcdir)/doc/modules/make-functions.js $(top_srcdir)/doc/modules/gtkbuilder/gtkbuilder.js > $(top_builddir)/doc/modules/gtkbuilder/gtkbuilder-funcs.xml
CLEANFILES=gtkbuilder-funcs.xml

gtkbuilderdocsdir = \
	$(docdir)/modules/gtkbuilder/html
gtkbuilderdocs_DATA = gtkbuilder-funcs.xml
