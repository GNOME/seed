#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object GtkButton\],\[object GtkButton\]\n\[object GtkButton\]\n\[object GtkButton\]
// STDERR:

Gtk = imports.gi.Gtk;

Gtk.init(Seed.argv);

b1 = new Gtk.Button();
b2 = new Gtk.Button();
vbox = new Gtk.VBox();
vbox.pack_start(b1);
vbox.pack_start(b2);

children = vbox.get_children();
print(children);
print(children[0]);
print(children[1]);
