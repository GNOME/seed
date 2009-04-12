#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object seed_union\]\n2\.000000\n300\.000000
// STDERR:

const Gdk = imports.gi.Gdk;

e = new Gdk.Event();
Seed.print(e);

e.expose.type = Gdk.EventType.EXPOSE;
Seed.print(e.expose.type);

r = new Gdk.Rectangle();
r.x = 300;
e.rectangle = r;
Seed.print(e.rectangle.x);
