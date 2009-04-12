#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:resize
// STDERR:

const Gtk = imports.gi.Gtk;
const GIRepository = imports.gi.GIRepository;

f = Gtk.Window.prototype.resize.info;

Seed.print(GIRepository.base_info_get_name(f));
