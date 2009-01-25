#!/usr/bin/env seed

Seed.import_namespace("Gtk");
Seed.import_namespace("Gdk");

with (Gtk)
{
	init(null, null);
	
	var quit = function ()
	{
		main_quit();
	}
	
	var w = new Window();
	w.signal.hide.connect(quit);
	
	var group = new AccelGroup();
	group.connect(Gdk.keyval_from_name("q"), 0, 0, quit);
	w.add_accel_group(group);
	
	var label = new Label({label: "Press Q to quit"});
	w.add(label);
	
	w.resize(300, 300);
	w.show_all();
	
	main();
}
