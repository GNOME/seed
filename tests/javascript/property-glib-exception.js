#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:\n\*\* \(seed:[0-9]+\): CRITICAL \*\*: Line 10 in .*\/property-glib-exception\.js: PropertyError value "3\.000000" of type `gdouble' is invalid or out of range for property `opacity' of type `gdouble'

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

window = new Gtk.Window();
window.opacity = 3;
