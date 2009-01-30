#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:5
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

HelloWindow = new GType({       
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
        var HelloSignalDefinition = {name: "hello",
                                     parameters: [GObject.TYPE_INT],
                                     return_type: GObject.TYPE_VALUE};

        hello_signal_id = klass.install_signal(HelloSignalDefinition);
    }
});

w = new HelloWindow();

w.signal.hello.connect(function(object, number, string)
                       {
                       	   //return number; // Neither of these work
                           return [GObject.TYPE_INT, 5];
                       });

// Seed.print dies if you try to print a GValue, or something...
Seed.print(w.signal.hello.emit(2));
//w.signal.hello.emit(2);
