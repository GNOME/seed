#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(null, null);

window = new Gtk.Window();
window.title="HI";
window.name="HI";
for (i = 0; i < 10000; i++)
{
	a = window.title;
	b = window.name;
}
