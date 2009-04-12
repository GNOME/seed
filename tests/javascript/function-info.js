#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:resize
// STDERR:

Gtk = imports.gi.Gtk;
GIRepository = imports.gi.GIRepository;

f = Gtk.Window.prototype.resize.info;

Seed.print(GIRepository.base_info_get_name(f));
