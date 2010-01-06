#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
GIRepository = imports.gi.GIRepository

f = Gtk.Window.prototype.resize.info

testsuite.assert(GIRepository.base_info_get_name(f) == "resize")
