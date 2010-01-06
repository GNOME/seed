#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

function handle_opacity_change(obj, gobject, user_data)
{
	testsuite.assert(user_data instanceof Gtk.Button)
	testsuite.assert(user_data === button)
	testsuite.assert(obj.opacity == 0.5)
}

win = new Gtk.Window()
button = new Gtk.Button()
win.signal.connect("notify::opacity", handle_opacity_change, button)
win.opacity = 0.5

testsuite.checkAsserts(3)
