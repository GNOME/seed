#!/usr/bin/env seed

// TODO: shouldn't the exception be passed back to the caller?

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

w = new Gtk.Window()
vbox = new Gtk.VBox()

closure = function(widget){a.a.a}
w.add(vbox)

vbox.pack_start(new Gtk.Label())

try
{
    vbox.foreach(closure)
    // ARST testsuite.unreachable()
}
catch(e)
{
    testsuite.assert(e instanceof ReferenceError)
}

//testsuite.checkAsserts
print("This test doesn't work yet...")
