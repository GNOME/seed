#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:2 Test\n\[object GtkWindow\]
// STDERR:

Seed.import_namespace("Gtk");
Gtk.init(null, null);

HelloWindow = new GType({       
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
        var HelloSignalDefinition = {name: "hello",
                                     parameters: [GObject.TYPE_INT,
                                                  GObject.TYPE_STRING],
                                     return_type: Gtk.Window.type};

        hello_signal_id = klass.install_signal(HelloSignalDefinition);
    }
});

w = new HelloWindow();

w.signal.hello.connect(function(object, number, string)
                       {
                           Seed.print(number + " " + string);
                           return new Gtk.Window();
                       });

Seed.print(w.signal.hello.emit(2, "Test"));

