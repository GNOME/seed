#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:

Seed.import_namespace("Gtk");

Gtk.init(null, null);

for(var i = 0; i < 100; i++)
{
    var button = new Gtk.Button({label: "Test!"});
}

