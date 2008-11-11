#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:1 1\n1 2\n2\.000000\n1\.000000
// STDERR:
Seed.import_namespace("Gtk");

Gtk.init(null, null);

w = new Gtk.Window();
l = new Gtk.Label();

Seed.print(w.__debug_ref_count() +" "+ l.__debug_ref_count());
w.add(l);
Seed.print(w.__debug_ref_count() +" "+ l.__debug_ref_count());
list = w.get_children();
l2 = list[0]
Seed.print(l2.__debug_ref_count());
w.remove(l);
Seed.print(l.__debug_ref_count());
