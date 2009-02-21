#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:\n\*\* \(seed:[0-9]+\): WARNING \*\*: Exception in signal handler. Line 11 in .*\/signal-exception.js: ReferenceError Left side of assignment is not a reference\.

Seed.import_namespace("Gtk");
Gtk.init(null, null);

w = new Gtk.Window();

w.signal.map.connect(function(){3 = undefined});
w.show();
