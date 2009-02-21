#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Hello World
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

Gtk.Window.prototype.hello = "Hello World";

a = new Gtk.Window();
Seed.print(a.hello);
