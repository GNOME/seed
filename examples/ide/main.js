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

Seed.include("ide-actions.js");
Seed.include("ide-sourceview.js");
Seed.include("ide-tab.js");
Seed.include("ide-tabheader.js");
Seed.include("ide-tabview.js");
Seed.include("ide-toolbar.js");
Seed.include("ide-messagearea.js");

// TODO: put this in a subclass of the window and give it a tab (read it out of the header)
function update_window(new_filename)
{
    var shortfilename = new_filename.split("/").slice(-1);
    
    if(new_filename != "")
        window.title = "Seed IDE - " + shortfilename;
    else
        window.title = "Seed IDE";
}

function current_tab()
{
	return tab_view.get_nth_page(tab_view.page);
}

var window = new Gtk.Window();
window.resize(700, 700);
window.signal.hide.connect(Gtk.main_quit);

init_ide_actions();

var toolbar = new IDEToolbar();

var tab_view = new IDETabView();
tab_view.create_tab("../ls.js");

var vbox = new Gtk.VBox();
vbox.pack_start(toolbar);
vbox.pack_start(tab_view, true, true);
vbox.show();

window.add(vbox);
window.show();

Gtk.main();
