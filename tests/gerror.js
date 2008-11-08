#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:GIoError Error opening file .* Is a directory
// STDERR:
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:
Seed.import_namespace("Gio")
try
{
	Gio.simple_write(".", "test");
}
catch (e)
{
	Seed.print(e.name+" "+e.message);
}
