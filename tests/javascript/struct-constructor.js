#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:100\.000000\n90\.000000\n80\.000000
// STDERR:

const Gdk = imports.gi.Gdk;

c = new Gdk.Color({red: 100});
Seed.print(c.red);
c = new Gdk.Color({red: 90, blue: 80});
Seed.print(c.red);
Seed.print(c.blue);
