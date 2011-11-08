#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
GObject = imports.gi.GObject
Gtk.init(Seed.argv);

var s = new Gtk.ListStore();

s.set_column_types(3, [GObject.TYPE_STRING, GObject.TYPE_INT, Gtk.Label.type]);

var ret = {};

s.append(ret);
s.set_value(ret.iter, 0, "Hi");
s.set_value(ret.iter, 1, [GObject.TYPE_INT, 10] );
l = new Gtk.Label.c_new("Hi");
s.set_value(ret.iter, 2, l);
