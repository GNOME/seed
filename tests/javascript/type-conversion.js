#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Can not convert Javascript value to boolean\nCan not convert Javascript value to int
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

actor = new Gtk.Window();
tests = [
	"actor.accept_focus = 'hello'",
	"actor.default_width = actor"];


for ( i in tests )
{
	try
	{
		eval(tests[i]);
	}
	catch (e)
	{
		print(e.message);
	}

}
