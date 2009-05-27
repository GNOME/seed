#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object GdkWindow\]\n\[object GdkWindow\]\n1\n200
// STDERR:

Gtk = imports.gi.Gtk;
Gdk = imports.gi.Gdk;
Gtk.init(Seed.argv);

function paint(wdg, evt)
{
	print(evt.expose.window);
	print(wdg.window);
	print(evt.expose.window === wdg.window);
	
	print(evt.expose.area.width);

	Gtk.main_quit();
	return false;
}

var win = new Gtk.Window();
win.signal.expose_event.connect(paint);
win.resize(200,200);
win.show();

Gtk.main();
