#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:\n\*\* \(seed:[0-9]+\): WARNING \*\*: Exception in closure .*

testsuite = imports.testsuite
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

w = new Gtk.Window();

//w.signal.map.connect(function(){3 = undefined});
//w.show();

print("This is another one of those tests that probably doesn't chain right yet")
