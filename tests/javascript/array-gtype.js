#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
GObject = imports.gi.GObject;

Gtk.init(Seed.argv);

try
{
    list = new Gtk.ListStore();
    list.set_column_types(2, [GObject.TYPE_STRING, GObject.TYPE_INT]);
}
catch(e)
{
    print(e)
    imports.testsuite.unreachable()
}
