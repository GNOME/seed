#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Constructor expects 1 argument, got 2\nConstructor expects object as argument
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

try
{
	w = new Gtk.Window();
}
catch (e)
{
	print(e.message);
}

try
{
	w = new Gtk.Window(1, 2);
}
catch (e)
{
	print(e.message);
}

try
{
	w = new Gtk.Window("safA");
}
catch (e)
{
	print(e.message);
}
