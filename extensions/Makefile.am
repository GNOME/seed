EXTRA_DIST= Gio.js Seed.js.in Gtk.js GObject.js Clutter.js Gst.js repl.js

extensiondir=$(datadir)/seed/extensions
extension_DATA = Gio.js Seed.js Gtk.js GObject.js Clutter.js Gst.js

repldir=$(datadir)/seed
repl_DATA = repl.js

Seed.js: Seed.js.in
	$(AM_V_GEN) $(SED) -e "s|%pkglibdir%|$(pkglibdir)|" -e "s|%pkgdatadir%|$(pkgdatadir)|" -e "s|%gnomejsdir%|$(GNOME_JS_DIR)|" $< > $@
