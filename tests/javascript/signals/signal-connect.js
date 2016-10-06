#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

function handle_opacity_change(obj, gobject, user_data)
{
	testsuite.assert(user_data instanceof Gtk.Button)
	testsuite.assert(user_data === button)
	// XXX: The following test got replaced because of a probably bug in Gtk.
	// widget_set_opacity(X) gets rounded, so when you ask for get_opacity the
	// number might not be the same
	//testsuite.assert(obj.opacity == 0.5)
	testsuite.assert(Math.abs(obj.opacity - 0.5) < 0.01)
}

win = new Gtk.Window()
button = new Gtk.Button()
win.signal.connect("notify::opacity", handle_opacity_change, button)
win.opacity = 0.5

testsuite.checkAsserts(3)
