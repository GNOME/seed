#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\{"Hello":"World"\}
// STDERR:

const Gtk = imports.gi.Gtk;
Gtk.init(null, null);

function userdata_test(widget, user_data)
{
    Seed.print(JSON.stringify(user_data));
}

w = new Gtk.Window();
w.signal.map.connect(userdata_test, {Hello: "World"});
w.show();
