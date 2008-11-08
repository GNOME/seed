#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Javascript number out of range of guchar\nCan not convert Javascript value to boolean\nCan not convert Javascript value to int
// STDERR:

Seed.import_namespace("Clutter");
Clutter.init(null, null);

actor = new Clutter.Rectangle();
tests = [
	"actor.opacity = 300",
	"actor.reactive = \"Nutrition\"",
	"actor.width = actor"];


for ( i in tests )
{
	try
	{
		eval(tests[i])
	}
	catch (e)
	{
		Seed.print(e.message);
	}
	
}