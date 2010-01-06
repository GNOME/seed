#!../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk

Gtk.init(Seed.argv)

Gtk.Window.prototype.hello = "Hello World"

a = new Gtk.Window()

testsuite.assert(a.hello == "Hello World")
