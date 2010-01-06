#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk

Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    init: function(self)
    {
	    testsuite.assert(self.test == 2)
	    testsuite.assert(self.testAgain == "arst")
    }
})

w = new HelloWindow({test: 2, testAgain: "arst"})

testsuite.checkAsserts(2)
