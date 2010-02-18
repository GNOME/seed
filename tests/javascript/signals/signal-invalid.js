#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

w = new Gtk.Window()

try
{
    a = w.signal.notasignal;
    testsuite.unreachable();
}
catch(e)
{
    testsuite.assert(e.message == "Failed to connect to notasignal. Invalid signal name.");
}

