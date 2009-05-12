#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:

Gtk = imports.gi.Gtk;
GObject = imports.gi.GObject;

Gtk.init(Seed.argv);

list = new Gtk.ListStore();
list.set_column_types(2, [GObject.TYPE_STRING, GObject.TYPE_INT]);
