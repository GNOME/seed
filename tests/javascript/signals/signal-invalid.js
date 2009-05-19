#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[undefined\]
// STDERR:
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

w = new Gtk.Window();
Seed.print(w.signal.notasignal);
