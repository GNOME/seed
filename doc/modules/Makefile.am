SUBDIRS=canvas multiprocessing readline sandbox sqlite gtkbuilder
EXTRA_DIST=make-functions.js book.xml version.xml.in

if ENABLE_GTK_DOC
html/index.html: version.xml book.xml sqlite/sqlite.xml sqlite/sqlite-funcs.xml readline/readline.xml readline/readline-funcs.xml make-functions.js gtkbuilder/gtkbuilder-funcs.xml gtkbuilder/gtkbuilder.xml sandbox/sandbox.xml sandbox/sandbox-funcs.xml
	mkdir -p html;
	gtkdoc-mkhtml "Seed" book.xml;
	mv *.html *.css *.png *.sgml *.devhelp* html

bookdir = $(docdir)/modules/html
book_DATA = html/index.html

install-data-local:
	cp -r html $(DESTDIR)$(docdir)/modules

CLEANFILES = html/*
endif 
