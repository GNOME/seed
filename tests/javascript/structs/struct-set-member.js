#!../../../src/seed

testsuite = imports.testsuite
Gdk = imports.gi.Gdk

color = new Gdk.Color()
testsuite.assert(color.red == 0)
color.red = 200
testsuite.assert(color.red == 200)
