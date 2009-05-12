#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:1
// STDERR:

Gtk = imports.gi.Gtk;
GObject = imports.gi.GObject;
Gtk.init(Seed.argv);

HelloWindowType = {
parent: Gtk.Window.type,
name: "HelloWindow",
class_init: function(klass, prototype)
{
	klass.c_install_property(GObject.param_spec_boolean("test",
													  "test property",
													  "A test property!",
													  false,
													  GObject.ParamFlags.CONSTRUCT | GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE));
},
init: function()
{
  Seed.print(this.test);
}};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow({test: true});


