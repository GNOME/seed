#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:
Seed.import_namespace("Gtk");
Seed.import_namespace("GObject");

Gtk.init(null, null);

list = new Gtk.ListStore();
list.set_column_types(2, [GObject.TYPE_STRING, GObject.TYPE_INT]);
