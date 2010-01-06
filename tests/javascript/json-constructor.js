#!../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
GLib = imports.gi.GLib

Gtk.init(Seed.argv)

window = new Gtk.Window({title: "JSON Win!"})
window.show_all()

testsuite.assert(window.title == "JSON Win!")
