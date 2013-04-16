#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
GIRepository = imports.gi.GIRepository;
 
f = Gtk.Window.prototype.resize.info;
 
//testsuite.assert(f.get_name() == "resize")
testsuite.assert(f == undefined)
