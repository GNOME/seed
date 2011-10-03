#!/usr/bin/env seed

testsuite = imports.testsuite;
Gdk = imports.gi.Gdk;
cairo = imports.gi.cairo;

r = new cairo.RectangleInt()
for (prop in r)
{ 
	testsuite.assert(prop == "x" || prop == "y" || prop == "width" || 
	                 prop == "height")
}

testsuite.checkAsserts(4)
