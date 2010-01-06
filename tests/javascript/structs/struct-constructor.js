#!../../../src/seed

testsuite = imports.testsuite
Gdk = imports.gi.Gdk

c = new Gdk.Color({red: 100})
testsuite.assert(c.red == 100)
c = new Gdk.Color({red: 90, blue: 80})
testsuite.assert(c.red == 90)
testsuite.assert(c.blue == 80)
