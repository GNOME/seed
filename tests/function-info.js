#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:resize
// STDERR:
Seed.import_namespace("Gtk");

f = Gtk.Window.prototype.resize.info

Seed.print(GIRepository.base_info_get_name(f));
