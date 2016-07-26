GLIB_COMPILE_RESOURCES=glib-compile-resources
js_files=Clutter.js \
          gjs/Gtk.js \
          gjs/GObject.js \
          gjs/Gio.js \
          gjs/GLib.js \
          GObject.js \
          Gio.js \
          Gst.js \
          GLib.js \
          Gtk.js \
          repl.js

resources=$(js_files) \
		  Seed.js

Seed.js: Seed.js.in
	$(AM_V_GEN) $(SED) -e "s|%pkglibdir%|$(pkglibdir)@SEED_GTK_VERSION@|" -e "s|%pkgdatadir%|$(pkgdatadir)@SEED_GTK_VERSION@|" -e "s|%gnomejsdir%|$(GNOME_JS_DIR)|" $< > $@

libseed-resources.c: libseed.gresources.xml Seed.js.in $(resources)
	$(GLIB_COMPILE_RESOURCES) libseed.gresources.xml --target=$@ --sourcedir=. --generate-source

all: libseed-resources.c

clean:
	rm libseed-resources.c

EXTRA_DIST = libseed.gresources.xml \
			 Seed.js.in \
			 $(js_files)
