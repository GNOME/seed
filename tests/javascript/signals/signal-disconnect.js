#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

myglobal = 0

function test()
{
    myglobal += 1234
}

w = new Gtk.Window()
id = w.signal.map.connect(test)
w.show_all()
w.hide()
w.signal.disconnect(id)
w.show_all()

testsuite.assert(myglobal == 1234)
