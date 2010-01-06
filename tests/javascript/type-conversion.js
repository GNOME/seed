#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

actor = new Gtk.Window()

try
{
    actor.accept_focus = 'hello'
    testsuite.unreachable()
}
catch(e)
{
    testsuite.assert(e.message = "Can not convert Javascript value to boolean")
}

try
{
    actor.default_width = actor
    testsuite.unreachable()
}
catch(e)
{
    testsuite.assert(e.message = "Can not convert Javascript value to int")
}

testsuite.checkAsserts(2)
