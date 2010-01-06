#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk

Gtk.init(Seed.argv)

window = new Gtk.Window()

try
{
    window.opacity = 3
}
catch(e)
{
    testsuite.assert(e.name == "PropertyError")
    testsuite.assert(e.message == "value \"3.000000\" of type `gdouble' is invalid or out of range for property `opacity' of type `gdouble'")
}

testsuite.checkAsserts(2)
