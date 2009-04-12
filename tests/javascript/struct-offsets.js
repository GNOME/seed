#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:17\.000000\n0\.000000
// STDERR:

Pango = imports.gi.Pango;

c = new Pango.Color();
c.red = 17;

Seed.print(c.red);
Seed.print(c.blue);
