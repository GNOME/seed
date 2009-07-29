#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

HelloWindowType = {
    parent: Gtk.Window.type,
    name: "HelloWindow"
};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();

InheritedWindowType = {
 parent: HelloWindow.type,
 name: "InheritedWindow"
};

InheritedWindow = new GType(InheritedWindowType);

b = new InheritedWindow();

