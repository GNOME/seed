EXTRA_DIST = \
	sqlite.js

sqlite-funcs.xml: sqlite.js
	../make-functions.js sqlite.js > sqlite-funcs.xml
CLEANFILES=sqlite-funcs.xml

sqlitedocsdir = \
	$(datadir)/doc/seed/modules/sqlite/html
sqlitedocs_DATA = sqlite-funcs.xml

#index.html: sqlite-funcs.xml
#	gtkdoc-mkhtml "sqlite" sqlite.xml;
#	mv *.html *.css *.png html

#sqlitedocs_DATA = html/*
