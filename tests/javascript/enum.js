#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk;

Gtk.init(Seed.argv);

b = new Gtk.Button()
b.relief = Gtk.ReliefStyle.NONE

testsuite.assert(Gtk.ReliefStyle.NONE == 2)
testsuite.assert(b.relief == 2)

