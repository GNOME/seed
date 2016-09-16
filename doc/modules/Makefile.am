SUBDIRS=canvas multiprocessing readline sandbox sqlite gtkbuilder
EXTRA_DIST=make-functions.js book.xml version.xml.in

if ENABLE_GTK_DOC
html/index.html: version.xml book.xml sqlite/sqlite.xml sqlite/sqlite-funcs.xml readline/readline.xml readline/readline-funcs.xml make-functions.js gtkbuilder/gtkbuilder-funcs.xml gtkbuilder/gtkbuilder.xml sandbox/sandbox.xml sandbox/sandbox-funcs.xml
	mkdir -p html;
	cd html && gtkdoc-mkhtml --path="$(abs_top_builddir)/doc/modules" "Seed" $(abs_top_srcdir)/doc/modules/book.xml;

bookdir = $(docdir)/modules/html
book_DATA = html/index.html

install-data-local:
	cp -r html $(DESTDIR)$(docdir)/modules

CLEANFILES = \
	html/* \
	html.stamp \
	$(NULL)

endif 

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)/modules

-include $(top_srcdir)/git.mk
