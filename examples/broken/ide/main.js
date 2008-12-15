#!/usr/bin/env seed

Seed.import_namespace("Vte");
Seed.import_namespace("Gtk");
Seed.import_namespace("GtkSource");
Seed.import_namespace("Gio");
Seed.import_namespace("Pango");
Seed.import_namespace("GConf");
Seed.import_namespace("Gdk");

Gtk.init(null, null);
GConf.init(null, null);

Seed.include("ide-window.js");
Seed.include("ide-actions.js");
Seed.include("ide-sourceview.js");
Seed.include("ide-tab.js");
Seed.include("ide-tabheader.js");
Seed.include("ide-tabview.js");
Seed.include("ide-toolbar.js");
Seed.include("ide-messagearea.js");

function current_tab()
{
	return window.tab_view.get_nth_page(window.tab_view.page);
}

window = new IDEWindow();

Gtk.main();
