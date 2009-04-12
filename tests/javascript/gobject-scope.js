#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:

const Gtk = imports.gi.Gtk;

Gtk.init(null, null);

for(var i = 0; i < 100; i++)
{
    var button = new Gtk.Button({label: "Test!"});
}

