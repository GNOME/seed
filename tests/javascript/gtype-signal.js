#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:Hello\nGoodbye
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

HelloWindowType = {       
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
	var HelloSignalDefinition = {name: "hello"};
	var GoodbyeSignalDefinition = {name: "goodbye"};
	
	
	hello_signal_id = klass.install_signal(HelloSignalDefinition);
	goodbye_signal_id = klass.install_signal(GoodbyeSignalDefinition);
    },
    init: function(instance)
    {
    }};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();

w.signal.hello.connect(function(){Seed.print("Hello")});
w.signal.goodbye.connect(function(){Seed.print("Goodbye")});

w.signal.hello.emit();
w.signal.goodbye.emit();
	  
