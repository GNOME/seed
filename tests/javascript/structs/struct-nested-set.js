#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object seed_union\]\n1\n300\n400
// STDERR:

Gdk = imports.gi.Gdk;

e = new Gdk.Event();
print(e);

e.type = Gdk.EventType.EXPOSE;
print(Gdk.EventType.EXPOSE == e.type);

r = new Gdk.Rectangle();
r.x = 300;
e.rectangle = r;
print(e.rectangle.x);
e.rectangle.x = 400;
print(e.rectangle.x);