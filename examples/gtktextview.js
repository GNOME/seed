#!/usr/local/bin/seed
Gtk = imports.gi.Gtk;
Gtk.init (null, null);

var window = new Gtk.Window();
var view = new Gtk.TextView();
var buffer = view.buffer;

var begin = new Gtk.TextIter();
buffer.get_start_iter (begin);

buffer.insert (begin, "Hello GtkTextView world.", -1);

window.add(view);
window.show_all();
window.resize (300, 300);

Gtk.main(0);

