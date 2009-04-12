#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Signal definition needs name property\nSignal definition needs name property
// STDERR:

const Gtk = imports.gi.Gtk;
Gtk.init(null, null);

HelloWindowType = {       
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
	var HelloSignalDefinition = {};
	var GoodbyeSignalDefinition = {name: 3};
	
	try {
		hello_signal_id = klass.install_signal(HelloSignalDefinition);
	    }
	catch (e){
		Seed.print(e.message);
	    }
	try
	    {
		goodbye_signal_id = klass.install_signal(GoodbyeSignalDefinition);
		hello_signal_id = klass.install_signal(HelloSignalDefinition);
	    }
	catch (e)
	    {
		Seed.print(e.message);
	}

    },
    init: function(klass)
    {
    }};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();

	  
