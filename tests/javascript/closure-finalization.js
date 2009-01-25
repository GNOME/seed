#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object GtkVBox\]
// STDERR:
Seed.import_namespace("Gtk");
Gtk.init(null, null);

w = new Gtk.Window();

// Closure will always be GCed at end of signal.
signal = function()
{
	w.foreach(function(widget){Seed.print(widget);});
};

vbox = new Gtk.VBox();

w.add(vbox);

w.signal.show.connect(signal);

w.show_all();
