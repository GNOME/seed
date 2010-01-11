#!../../../src/seed

testsuite = imports.testsuite;
Gtk = imports.gi.Gtk;

Gtk.init(Seed.argv);

var iconTheme = Gtk.IconTheme.get_default();

// returns NULL if icon not found
var iconInfo = iconTheme.lookup_icon("fake icon name", 32);

testsuite.assert(iconInfo === null)

