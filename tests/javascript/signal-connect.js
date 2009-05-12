#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object GtkButton\]\n1\n0\.5
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

function handle_opacity_change(obj, gobject, user_data)
{
	Seed.print(user_data);
	Seed.print(user_data === button);
	Seed.print(obj.opacity);
}

win = new Gtk.Window();
button = new Gtk.Button();
win.signal.connect("notify::opacity", handle_opacity_change, button);
win.opacity = 0.5;

