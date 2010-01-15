#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

HelloWindowType = {
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
		prototype = notAVariable.notAProperty
    },
    init: function(klass)
    {
    }
}

try
{
    HelloWindow = new GType(HelloWindowType)
    w = new HelloWindow()
}
catch(e)
{
    testsuite.assert(e instanceof ReferenceError)
}

testsuite.checkAsserts(1)

