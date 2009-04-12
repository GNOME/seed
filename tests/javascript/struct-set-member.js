#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:0\.000000\n200\.000000
// STDERR:

Gdk = imports.gi.Gdk;

color = new Gdk.Color();
Seed.print(color.red);
color.red = 200;
Seed.print(color.red);
