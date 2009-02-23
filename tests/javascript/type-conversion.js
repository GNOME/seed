#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Can not convert Javascript value to boolean\nCan not convert Javascript value to int
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

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
		Seed.print(e.message);
	}
	
}
