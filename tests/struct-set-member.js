#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:0\.000000\n200\.000000
// STDERR:
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:
Seed.import_namespace("Gdk");

color = new Gdk.Color();
Seed.print(color.red);
color.red = 200;
Seed.print(color.red);