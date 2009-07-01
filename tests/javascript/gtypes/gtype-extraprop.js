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
	print(self.test);
    }};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow({test: 1});

w.show();

