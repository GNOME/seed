#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:In signal
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

function expects_test(widget, user_data)
{
    print("In signal");
}

w = new Gtk.Window();
w.signal.map.connect(expects_test);
w.show();
