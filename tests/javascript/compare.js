#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:0.000000\n1.000000
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(null, null);

var a = new Gtk.Button();
var b = new Gtk.Button();
var c = a;

Seed.print(a.equals(b));
Seed.print(a.equals(c));
