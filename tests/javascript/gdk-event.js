#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gdk = imports.gi.Gdk
Gtk.init(Seed.argv)

function paint(wdg, evt)
{
    testsuite.assert(evt.expose.window instanceof Gdk.Window)
    testsuite.assert(wdg.window instanceof Gdk.Window)
    testsuite.assert(evt.expose.window == wdg.window)
    testsuite.assert(evt.expose.area.width == 200)

	Gtk.main_quit()

	return false
}

var win = new Gtk.Window()
win.signal.expose_event.connect(paint)
win.resize(200,200)
win.show()

Gtk.main()

testsuite.checkAsserts(4)
