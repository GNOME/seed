#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

function expects_test(widget, user_data)
{
    testsuite.assert(widget == w)
    testsuite.assert(user_data == null)
}

w = new Gtk.Window()
w.signal.map.connect(expects_test)
w.show()

testsuite.checkAsserts(2)
