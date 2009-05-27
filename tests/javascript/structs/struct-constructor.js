#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:100\n90\n80
// STDERR:

Gdk = imports.gi.Gdk;

c = new Gdk.Color({red: 100});
print(c.red);
c = new Gdk.Color({red: 90, blue: 80});
print(c.red);
print(c.blue);
