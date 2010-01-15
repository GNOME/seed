#!/usr/bin/env seed

// TODO: this test doesn't do anything

Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

window = new Gtk.Window();
window.title="HI";
window.name="HI";
for (i = 0; i < 10000; i++)
{
	a = window.title;
	b = window.name;
}
