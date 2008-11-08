#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:In signal
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

function expects_test(widget, user_data)
{
    Seed.print("In signal");
}

w = new Gtk.Window();
w.signal.map.connect(expects_test);
w.show();