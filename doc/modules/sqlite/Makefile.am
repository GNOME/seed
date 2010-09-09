EXTRA_DIST = \
	sqlite.js sqlite.xml

sqlite-funcs.xml: $(top_builddir)/src/seed $(top_srcdir)/doc/modules/sqlite/sqlite.js
	$(top_builddir)/src/seed $(top_srcdir)/doc/modules/make-functions.js $(top_srcdir)/doc/modules/sqlite/sqlite.js > $(top_builddir)/doc/modules/sqlite/sqlite-funcs.xml
CLEANFILES=sqlite-funcs.xml

sqlitedocsdir = \
	$(docdir)/modules/sqlite/html
sqlitedocs_DATA = sqlite-funcs.xml

#index.html: sqlite-funcs.xml
#	gtkdoc-mkhtml "sqlite" sqlite.xml;
#	mv *.html *.css *.png html

#sqlitedocs_DATA = html/*
