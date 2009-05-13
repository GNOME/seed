#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Signal handler called
// STDERR:
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

function test(){
    Seed.print("Signal handler called");
}

w = new Gtk.Window();
id = w.signal.map.connect(test);
w.show_all();
w.hide();
w.signal.disconnect(id);
w.show_all();
