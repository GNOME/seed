#!../../../src/seed

testsuite = imports.testsuite
Gdk = imports.gi.Gdk

r = new Gdk.Rectangle()
for (prop in r)
{
	testsuite.assert(prop == "x" || prop == "y" || prop == "width" || 
	                 prop == "height" || prop == "intersect" || prop == "union")
}

testsuite.checkAsserts(6)
