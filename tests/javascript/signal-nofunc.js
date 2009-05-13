#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Signal connection requires a function as first argument
// STDERR:
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

w = new Gtk.Window();
try{
    w.signal.map.connect(3);
}
catch (e){
    Seed.print(e.message);
}
