#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
GObject = imports.gi.GObject
Gtk.init(Seed.argv);

var s = new Gtk.ListStore();

s.set_column_types(2, [GObject.TYPE_STRING, GObject.TYPE_INT]);

var ret = {};

s.append(ret);
s.set_value(ret.iter, 0, "Hi");
s.set_value(ret.iter, 1, [GObject.TYPE_INT, 10] );
