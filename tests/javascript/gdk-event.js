#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gdk = imports.gi.Gdk
Gtk.init(Seed.argv)

function paint(wdg, cr)
{
    
    
    testsuite.assert(wdg.window instanceof Gdk.Window)
    var sz = wdg.get_size();
    testsuite.assert(sz.width == 200)

	Gtk.main_quit()

	return false
}

var win = new Gtk.Window()
win.signal.draw.connect(paint)
win.resize(200,200)
win.show()

Gtk.main()

testsuite.checkAsserts(2)
