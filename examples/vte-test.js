#!/usr/local/bin/seed
Seed.import_namespace("Gtk","2.0");
Seed.import_namespace("Vte");

Gtk.init(null, null);

function childquit(terminal)
{
    Gtk.main_quit();
}

function updatetitle(terminal)
{
    this.set_title(terminal.get_window_title());
}


var window = new Gtk.Window();

var vte = new Vte.Terminal();
vte.fork_command("/bin/bash");
vte.signal_child_exited.connect(childquit);
vte.signal_window_title_changed.connect(updatetitle, window);

window.add(vte);

window.show_all();

Gtk.main();

