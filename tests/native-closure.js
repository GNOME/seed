#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:\[object GtkLabel\]\n\[object GtkButton\]
// STDERR:
Seed.import_namespace("Gtk")
Gtk.init(null, null);

w = new Gtk.Window();
vbox = new Gtk.VBox();

closure = Seed.closure_native(function(widget){Seed.print(widget)}, 
			      Gtk.Callback);
w.add(vbox);

vbox.pack_start(new Gtk.Label());
vbox.pack_start(new Gtk.Button());

vbox.foreach(closure);
