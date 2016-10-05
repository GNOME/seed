#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk

f = Gtk.Window.prototype.resize.info;

testsuite.assert(f.get_name() == "resize")
