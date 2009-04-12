#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:\n\*\* \(seed:[0-9]+\): WARNING \*\*: Exception in closure marshal\. Line 12 in .*\/native-closure-exception\.js: ReferenceError Can't find variable: a

const Gtk = imports.gi.Gtk;
Gtk.init(null, null);

w = new Gtk.Window();
vbox = new Gtk.VBox();

closure = function(widget){a.a.a;};
w.add(vbox);

vbox.pack_start(new Gtk.Label());

vbox.foreach(closure);
