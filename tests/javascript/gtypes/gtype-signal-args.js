#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:2 Test\n\[object GtkWindow\]
// STDERR:

Gtk = imports.gi.Gtk;
GObject = imports.gi.GObject;
Gtk.init(Seed.argv);

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    signals: [{name: "hello",
	       parameters: [GObject.TYPE_INT,
			    GObject.TYPE_STRING],
	       return_type: Gtk.Window.type}]
});

w = new HelloWindow();

w.signal.hello.connect(function(object, number, string)
                       {
                           var win = new Gtk.Window();
                           print(number + " " + string);
                           return win;
                       });
print(w.signal.hello.emit(2, "Test"));

