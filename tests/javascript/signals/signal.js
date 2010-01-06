#!../../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

function mapped(window)
{
    testsuite.assert(window == w)
}

w = new Gtk.Window()
w.signal.map.connect(mapped)

w.show_all()

testsuite.checkAsserts(1)
