#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
JSON = imports.JSON

Gtk.init(Seed.argv)

win = new Gtk.Window()

info = Seed.introspect(win.set_opacity)
 
testsuite.assert(info.name == "set_opacity")
testsuite.assert(info.return_type == "void")
testsuite.assert(info.args[0].type == "gdouble")
