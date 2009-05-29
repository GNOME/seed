EXTRA_DIST = \
	sqlite.js sqlite.xml

sqlite-funcs.xml: $(top_builddir)/src/seed sqlite.js
	$(top_builddir)/src/seed ../make-functions.js sqlite.js > $(top_builddir)/doc/modules/sqlite/sqlite-funcs.xml
CLEANFILES=sqlite-funcs.xml

sqlitedocsdir = \
	$(datadir)/doc/seed/modules/sqlite/html
sqlitedocs_DATA = sqlite-funcs.xml

#index.html: sqlite-funcs.xml
#	gtkdoc-mkhtml "sqlite" sqlite.xml;
#	mv *.html *.css *.png html

#sqlitedocs_DATA = html/*
