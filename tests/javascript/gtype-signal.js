#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Hello\nGoodbye
// STDERR:

Gtk = imports.gi.Gtk;
Gtk.init(null, null);

HelloWindowType = {
    parent: Gtk.Window.type,
    name: "HelloWindow",
    signals: [{name: "hello"}, {name: "goodbye"}]
};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();

w.signal.hello.connect(function(){Seed.print("Hello");});
w.signal.goodbye.connect(function(){Seed.print("Goodbye");});

w.signal.hello.emit();
w.signal.goodbye.emit();

