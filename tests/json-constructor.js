#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:JSON Win!
// STDERR:

Seed.import_namespace("Gtk");
Seed.import_namespace("GLib");

Gtk.init(null, null);
window = new Gtk.Window({"title":"JSON Win!"});
window.show_all();

Seed.print(window.title);

