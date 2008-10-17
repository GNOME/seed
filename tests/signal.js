#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:Window mapped.
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

function mapped(window)
{
    Seed.print("Window mapped.");
}

w = new Gtk.Window();
w.signal_map.connect(mapped);

w.show_all();
