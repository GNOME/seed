SUBDIRS=canvas multiprocessing readline sandbox sqlite

if ENABLE_GTK_DOC
html/index.html:
	mkdir -p html;
	gtkdoc-mkhtml "Seed" book.xml;
	mv *.html *.css *.png *.sgml *.devhelp* html


bookdir = $(datadir)/doc/seed/modules/html
book_DATA = html/index.html

install-data-local:
	cp -r html $(datadir)/doc/seed/modules
endif 


