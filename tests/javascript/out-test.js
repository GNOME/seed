#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:5 100
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

w = new Gtk.Window({width_request: 5, height_request: 100});
o1 = { };
o2 = { };

w.get_size_request(o1, o2);
Seed.print(o1.value + " " +  o2.value);
