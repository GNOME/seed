#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Window mapped.
// STDERR:

const Gtk = imports.gi.Gtk;
Gtk.init(null, null);

function mapped(window)
{
    Seed.print("Window mapped.");
}

w = new Gtk.Window();
w.signal.map.connect(mapped);

w.show_all();
