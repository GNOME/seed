EXTRA_DIST = \
	readline.js readline.xml

readline-funcs.xml: $(top_builddir)/src/seed readline.js
	$(top_builddir)/src/seed ../make-functions.js readline.js > readline-funcs.xml
CLEANFILES=readline-funcs.xml

readlinedocsdir = \
	$(datadir)/doc/seed/modules/readline/html
readlinedocs_DATA = readline-funcs.xml

#index.html: readline-funcs.xml
#	gtkdoc-mkhtml "readline" readline.xml;
#	mv *.html *.css *.png html

#readlinedocs_DATA = html/*
