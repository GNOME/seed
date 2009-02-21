#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:255\.000000\n0\.000000
// STDERR:

Seed.import_namespace("Clutter");

c = new Clutter.Color();
Clutter.color_parse("red", c);

Seed.print(c.red);
Seed.print(c.blue);
