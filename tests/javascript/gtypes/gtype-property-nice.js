#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:1\n1\n0
// STDERR:

GObject = imports.gi.GObject;
Gtk = imports.gi.Gtk;
Gtk.init(Seed.argv);

HelloWindowType = {
    parent: Gtk.Window.type,
    name: "HelloWindow",
    properties: [{name: "randomproperty",
		  type: GObject.TYPE_BOOLEAN,
		  nick: "randomproperty",
		  blurb: "longer blurb about property",
		  default_value: true,
		  flags: (GObject.ParamFlags.CONSTRUCT | GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE)}],
    init: function(klass)
    {

    }};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();

print(w.randomproperty);
w.randomproperty = 5;
print(w.randomproperty);
w.randomproperty = 0;
print(w.randomproperty);
