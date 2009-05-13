#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:100\n90\n80
// STDERR:

Gdk = imports.gi.Gdk;

c = new Gdk.Color({red: 100});
Seed.print(c.red);
c = new Gdk.Color({red: 90, blue: 80});
Seed.print(c.red);
Seed.print(c.blue);
