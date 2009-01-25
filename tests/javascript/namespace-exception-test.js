#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Typelib file for namespace 'CoreAnimation' \(any version\) not found\nTypelib file for namespace 'Gtk', version '3\.0' not found
// STDERR:

try
{
	Seed.import_namespace("CoreAnimation");
}
catch(e)
{
	Seed.print(e.message);
}

try
{
	Seed.import_namespace("Gtk","3.0");
}
catch(e)
{
	Seed.print(e.message);
}
