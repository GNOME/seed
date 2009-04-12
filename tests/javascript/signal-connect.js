#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object GtkButton\]\n1\.000000\n0\.500000
// STDERR:

const Gtk = imports.gi.Gtk;
Gtk.init(null, null);

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

