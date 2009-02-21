#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:x\ny\nwidth\nheight\nintersect\nunion
// STDERR:

Seed.import_namespace("Gdk");

r = new Gdk.Rectangle();
for (prop in r)
{
	Seed.print(prop);
}
