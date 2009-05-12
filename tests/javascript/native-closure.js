#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object GtkLabel\]\n\[object GtkButton\]
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

w = new Gtk.Window();
vbox = new Gtk.VBox();

closure = function(widget){Seed.print(widget);};
w.add(vbox);

vbox.pack_start(new Gtk.Label());
vbox.pack_start(new Gtk.Button());

vbox.foreach(closure);
