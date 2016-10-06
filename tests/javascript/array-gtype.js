#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
GObject = imports.gi.GObject;

Gtk.init(Seed.argv);

try
{
    list = new Gtk.ListStore();
    list.set_column_types([GObject.TYPE_STRING, GObject.TYPE_INT]);
}
catch(e)
{
    print(e.message)
    imports.testsuite.unreachable()
}
