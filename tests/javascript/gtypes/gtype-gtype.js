#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Hi\nHi\nbye
// STDERR:
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

HelloWindowType = {
 parent: Gtk.Window.type,
 name: "HelloWindow",
 init: function(){
    print("Hi");
  }
};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();

InheritedWindowType = {
 parent: HelloWindow.type,
 name: "InheritedWindow",
 init: function(){
    print("bye");
  }
};

InheritedWindow = new GType(InheritedWindowType);

b = new InheritedWindow();

