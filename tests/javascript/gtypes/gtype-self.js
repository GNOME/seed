#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    init: function(self)
    {
	    testsuite.assert(this == self)
    }
})

w = new HelloWindow()

testsuite.checkAsserts(1)
