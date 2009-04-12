#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Hello World
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(null, null);

Gtk.Window.prototype.hello = "Hello World";

a = new Gtk.Window();
Seed.print(a.hello);
