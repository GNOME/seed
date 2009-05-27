#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:5 100
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

w = new Gtk.Window({width_request: 5, height_request: 100});
o1 = { };
o2 = { };

w.get_size_request(o1, o2);
print(o1.value + " " +  o2.value);
