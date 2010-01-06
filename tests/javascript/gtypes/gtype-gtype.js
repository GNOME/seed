#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    init: function(self)
    {
        self.id = 5
    }
})

InheritedWindow = new GType({
    parent: HelloWindow.type,
    name: "InheritedWindow",
    init: function(self)
    {
        self.id = 10
    }
})

w = new HelloWindow()
b = new InheritedWindow()

testsuite.assert(w.id == 5)
testsuite.assert(b.id == 10)
