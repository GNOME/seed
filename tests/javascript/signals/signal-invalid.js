#!../../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

w = new Gtk.Window()

testsuite.assert(w.signal.notasignal == undefined)
