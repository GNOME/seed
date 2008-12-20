#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:2.000000
// STDERR:
 
Seed.import_namespace("Gtk");
Gtk.init(null, null);

b = new Gtk.Button();
b.relief = Gtk.ReliefStyle.None;

Seed.print(Gtk.ReliefStyle.None);


