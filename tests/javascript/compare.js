#!../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk;

Gtk.init(Seed.argv);

var a = new Gtk.Button();
var b = new Gtk.Button();
var c = a;

testsuite.assert(a != b);
testsuite.assert(c == a);
