#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:No such namespace: CoreAnimation\nNo such namespace: Gtk \(version 3\.0\)
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