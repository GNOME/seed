#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:17\n0
// STDERR:

Pango = imports.gi.Pango;

c = new Pango.Color();
c.red = 17;

print(c.red);
print(c.blue);
