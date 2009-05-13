#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object seed_union\]\n2\n300
// STDERR:

Gdk = imports.gi.Gdk;

e = new Gdk.Event();
Seed.print(e);

e.expose.type = Gdk.EventType.EXPOSE;
Seed.print(e.expose.type);

r = new Gdk.Rectangle();
r.x = 300;
e.rectangle = r;
Seed.print(e.rectangle.x);
