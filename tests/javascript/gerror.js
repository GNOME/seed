#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:GIoError Error opening file .* Is a directory
// STDERR:

Gio = imports.gi.Gio;

try
{
	Gio.simple_write(".", "test");
}
catch (e)
{
	Seed.print(e.name+" "+e.message);
}
