#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Window mapped.
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

function mapped(window)
{
    Seed.print("Window mapped.");
}

w = new Gtk.Window();
w.signal.map.connect(mapped);

w.show_all();
