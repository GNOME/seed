#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\{"Hello":"World"\}
// STDERR:

Gtk = imports.gi.Gtk;
JSON = imports.JSON;

Gtk.init(Seed.argv);

function userdata_test(widget, user_data)
{
    Seed.print(JSON.stringify(user_data));
}

w = new Gtk.Window();
w.signal.map.connect(userdata_test, {Hello: "World"});
w.show();
