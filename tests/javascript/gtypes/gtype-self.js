#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:1
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

HelloWindowType = {
    parent: Gtk.Window.type,
    name: "HelloWindow",
    init: function(self){
	print(this==self);
    }};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();

w.show();

