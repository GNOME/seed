#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:\[object GdkWindow\]\n\[object GdkWindow\]\n1\.000000\n200\.000000
// STDERR:

Seed.import_namespace("Gtk");
Seed.import_namespace("Gdk");
Gtk.init(null, null);

function paint(wdg, evt)
{
	Seed.print(evt.expose.window);
	Seed.print(wdg.window);
	Seed.print(evt.expose.window === wdg.window);
	
	Seed.print(evt.expose.area.width);

	Gtk.main_quit();
	return false;
}

var win = new Gtk.Window();
win.signal.expose_event.connect(paint);
win.resize(200,200);
win.show();

Gtk.main();
