#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    init: function()
    {
    }
})

w = new HelloWindow()

InheritedWindow = new GType({
    parent: HelloWindow.type,
    name: "InheritedWindow",
    class_init: function(klass, prototype)
    {
        testsuite.assert(1)
    },
    init: function()
    {
    }
})

b = new InheritedWindow()
testsuite.checkAsserts(1)
