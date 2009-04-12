#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:\n\*\* \(seed:[0-9]+\): WARNING \*\*: Exception in class init closure\. Line 14 in .*\/gtype-class-init-exception\.js: ReferenceError Can't find variable: notAVariable

const Gtk = imports.gi.Gtk;
Gtk.init(null, null);

HelloWindowType = {
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
		prototype = notAVariable.notAProperty;
    },
    init: function(klass)
    {
    }};

HelloWindow = new GType(HelloWindowType);
w = new HelloWindow();
	  
