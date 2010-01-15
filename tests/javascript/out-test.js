#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk

Gtk.init(Seed.argv)

w = new Gtk.Window({width_request: 5, height_request: 100})
o1 = { }
o2 = { }

w.get_size_request(o1, o2)

testsuite.assert(o1.value == 5)
testsuite.assert(o2.value == 100)
