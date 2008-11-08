#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:\{"Hello":"World"\}
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

function this_test(widget)
{
    Seed.print(JSON.stringify(this));
}

w = new Gtk.Window();
w.signal.map.connect(this_test, {Hello: "World"});
w.show();