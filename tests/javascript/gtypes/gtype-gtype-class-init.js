#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Class init
// STDERR:
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

HelloWindowType = {
 parent: Gtk.Window.type,
 name: "HelloWindow",
 init: function(){
  }
};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();

InheritedWindowType = {
 parent: HelloWindow.type,
 name: "InheritedWindow",
 class_init: function(klass, prototype) {print("Class init")},
 init: function(){
  }
};

InheritedWindow = new GType(InheritedWindowType);

b = new InheritedWindow();

