#!../../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk;
JSON = imports.JSON;

Gtk.init(Seed.argv);

function userdata_test(widget, user_data)
{
    testsuite.assert(user_data.Hello == "World")
}

w = new Gtk.Window();
w.signal.map.connect(userdata_test, {Hello: "World"});
w.show();

testsuite.checkAsserts(1)
