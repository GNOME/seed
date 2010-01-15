#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk

Gtk.init(Seed.argv)

b1 = new Gtk.Button()
b2 = new Gtk.Button()
vbox = new Gtk.VBox()
vbox.pack_start(b1)
vbox.pack_start(b2)

children = vbox.get_children()

testsuite.assert(children[0] instanceof Gtk.Button)
testsuite.assert(children[1] instanceof Gtk.Button)
testsuite.assert(children[0] != children[1])
testsuite.assert(children.length == 2)

