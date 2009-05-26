#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:0\n1
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

var a = new Gtk.Button();
var b = new Gtk.Button();
var c = a;

Seed.print(a == b);
Seed.print(c == a);
