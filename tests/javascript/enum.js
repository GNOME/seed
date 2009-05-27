#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:2
// STDERR:
 
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

b = new Gtk.Button();
b.relief = Gtk.ReliefStyle.NONE;

print(Gtk.ReliefStyle.NONE);


