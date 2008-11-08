#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Constructor expects 1 argument, got 2\nConstructor expects object as argument
// STDERR:
Seed.import_namespace("Gtk");
Gtk.init(null, null);

try
{
	w = new Gtk.Window();
}
catch (e)
{
	Seed.print(e.message);
}

try
{
	w = new Gtk.Window(1, 2);
}
catch (e)
{
	Seed.print(e.message);
}

try
{
	w = new Gtk.Window("safA");
}
catch (e)
{
	Seed.print(e.message);
}
