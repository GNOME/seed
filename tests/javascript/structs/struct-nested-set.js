#!/usr/bin/env seed

testsuite = imports.testsuite;
Gdk = imports.gi.Gdk;
cairo = imports.gi.cairo;

e = new Gdk.Event();
testsuite.assert(e.toString() == "[object seed_union]");

e.type = Gdk.EventType.EXPOSE;
testsuite.assert(Gdk.EventType.EXPOSE == e.type);

r = new cairo.RectangleInt()
r.x = 300
e.rectangle = r
testsuite.assert(e.rectangle.x == 300)
e.rectangle.x = 400
testsuite.assert(e.rectangle.x == 400)
