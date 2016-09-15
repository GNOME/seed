js_files = \
	Clutter.js \
	gjs/Gtk.js \
	gjs/GObject.js \
	gjs/Gio.js \
	gjs/GLib.js \
	GObject.js \
	Gio.js \
	Gst.js \
	GLib.js \
	Gtk.js \
	repl.js \
	$(NULL)

resources = \
	$(js_files) \
	Seed.js \
	$(NULL)

Seed.js: Seed.js.in
	$(AM_V_GEN) $(SED) -e "s|%pkglibdir%|$(pkglibdir)@SEED_GTK_VERSION@|" -e "s|%pkgdatadir%|$(pkgdatadir)@SEED_GTK_VERSION@|" -e "s|%gnomejsdir%|$(GNOME_JS_DIR)|" $< > $@

libseed-resources.c: libseed.gresources.xml Seed.js $(resources)
	$(GLIB_COMPILE_RESOURCES) $(top_srcdir)/jsextensions/libseed.gresources.xml --target=$@ --sourcedir=$(top_srcdir)/jsextensions --sourcedir=. --generate-source

BUILT_SOURCES = \
	libseed-resources.c \
	Seed.js \
	$(NULL)

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
	libseed.gresources.xml \
	Seed.js.in \
	$(js_files)
