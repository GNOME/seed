#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:\n\*\* \(seed:[0-9]+\): CRITICAL \*\*: PropertyError. value "3.000000" of type `gdouble' is invalid or out of range for property `opacity' of type `gdouble' in ./property-glib-exception.js at line 10

Seed.import_namespace("Gtk");
Gtk.init(null, null);

window = new Gtk.Window();
window.opacity = 3;
