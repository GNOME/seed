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
                           return 5;
                       });

Seed.print(w.signal.hello.emit(2));

