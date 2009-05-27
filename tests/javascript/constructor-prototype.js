#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Hello World
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

Gtk.Window.prototype.hello = "Hello World";

a = new Gtk.Window();
print(a.hello);
