#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Gdk = imports.gi.Gdk;

Gtk.init(Seed.argv);

var w = new Gtk.Window();
w.signal.hide.connect(Gtk.main_quit);

var group = new Gtk.AccelGroup();
group.connect(Gdk.keyval_from_name("q"), 0, 0, Gtk.main_quit);
w.add_accel_group(group);

var label = new Gtk.Label({label: "Press Q to quit"});
w.add(label);

w.resize(300, 300);
w.show_all();

Gtk.main();
