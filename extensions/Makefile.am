EXTRA_DIST= GLib.js Gio.js Seed.js.in Gtk.js GObject.js Clutter.js Gst.js repl.js

extensiondir=$(datadir)/seed@SEED_GTK_VERSION@/extensions
extension_DATA = GLib.js Gio.js Seed.js Gtk.js GObject.js Clutter.js Gst.js

repldir=$(datadir)/seed@SEED_GTK_VERSION@
repl_DATA = repl.js

Seed.js: Seed.js.in
	$(AM_V_GEN) $(SED) -e "s|%pkglibdir%|$(pkglibdir)@SEED_GTK_VERSION@|" -e "s|%pkgdatadir%|$(pkgdatadir)@SEED_GTK_VERSION@|" -e "s|%gnomejsdir%|$(GNOME_JS_DIR)|" $< > $@
