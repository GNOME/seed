SUBDIRS=canvas multiprocessing readline sandbox sqlite

if ENABLE_GTK_DOC
html/index.html: book.xml sqlite/sqlite.xml sqlite/sqlite-funcs.xml readline/readline.xml readline/readline-funcs.xml
	mkdir -p html;
	gtkdoc-mkhtml "Seed" book.xml;
	mv *.html *.css *.png *.sgml *.devhelp* html


bookdir = $(datadir)/doc/seed/modules/html
book_DATA = html/index.html

install-data-local:
	cp -r html $(datadir)/doc/seed/modules

CLEANFILES = html/*
endif 



