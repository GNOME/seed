#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:JSON Win!
// STDERR:

Gtk = imports.gi.Gtk;
GLib = imports.gi.GLib;

Gtk.init(null, null);
window = new Gtk.Window({title: "JSON Win!"});
window.show_all();

Seed.print(window.title);

