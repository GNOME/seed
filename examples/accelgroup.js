#!/usr/bin/env seed
Seed.import_namespace("Gtk")
Seed.import_namespace("Gdk");
Seed.import_namespace("GLib");

with (Gtk)
{
	init(null, null);
	
	w = new Window();
	group = new AccelGroup();
	group.connect(Gdk.keyval_from_name("q"), 
				  0, 
				  0, 
				  function()
				  {
					Seed.quit();
				  });
	w.add_accel_group(group);
	
	label = new Label({label: "Press Q to quit"});
	w.add(label);
	w.width_request = w.height_request = 300;
	w.show_all();
	
	main();
}
