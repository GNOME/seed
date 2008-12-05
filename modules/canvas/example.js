#!/usr/local/bin/seed
Seed.import_namespace("Gdk");
Seed.import_namespace("Gtk");
Seed.import_namespace("Canvas");


Gtk.init(null, null);

w = new Gtk.Window();
w.show_all();

cr = Gdk.cairo_create(w.window);
c = new Canvas.CairoCanvas(cr);
Seed.print(c);




