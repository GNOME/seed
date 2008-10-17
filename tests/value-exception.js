#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:\n\*\* \(seed:[0-9]*\): CRITICAL \*\*: InvalidPropertyValue. Not able to set property opacity on object of type GtkWindow. Expected type: gdouble.

Seed.import_namespace("GLib");
Seed.import_namespace("Gtk");
Gtk.init(null, null);
window = new Gtk.Window();
window.opacity = "Hello World!";
