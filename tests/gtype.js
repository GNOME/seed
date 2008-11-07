#!/usr/local/bin/seed
// Returns: 0
// STDIN:
// STDOUT:In constructor for \[object HelloWindow\]\nIn map, verifying widget\.title : Hello!
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

HelloWindowType = {
    parent: Gtk.Window,
    name: "HelloWindow",
    instance_init: function(widget, klass)
    {
	widget.title = "Hello!";
	Seed.print("In constructor for " + widget);
    }};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();
w.signal.map.connect(
		     function(widget)
		     {
			 Seed.print("In map, verifying widget.title : " 
				    + widget.title)
		     });
w.show();
	  
