#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:2
// STDERR:
 
Gtk = imports.gi.Gtk;
Gtk.init(null, null);

b = new Gtk.Button();
b.relief = Gtk.ReliefStyle.NONE;

Seed.print(Gtk.ReliefStyle.NONE);


