#!/usr/bin/env seed

testsuite = imports.testsuite
Pango = imports.gi.Pango

c = new Pango.Color()
c.red = 17

testsuite.assert(c.red == 17)
testsuite.assert(c.blue == 0)
