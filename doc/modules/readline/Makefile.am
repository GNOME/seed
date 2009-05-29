EXTRA_DIST = \
	readline.js readline.xml

readline-funcs.xml: $(top_builddir)/src/seed $(top_srcdir)/doc/modules/readline/readline.js
	$(top_builddir)/src/seed $(top_srcdir)/doc/modules/make-functions.js $(top_srcdir)/doc/modules/readline/readline.js > $(top_builddir)/doc/modules/readline/readline-funcs.xml
CLEANFILES=readline-funcs.xml

readlinedocsdir = \
	$(datadir)/doc/seed/modules/readline/html
readlinedocs_DATA = readline-funcs.xml

#index.html: readline-funcs.xml
#	gtkdoc-mkhtml "readline" readline.xml;
#	mv *.html *.css *.png html

#readlinedocs_DATA = html/*
