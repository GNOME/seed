#!/usr/bin/env seed
// Returns: 0
// STDIN:
// STDOUT:
// STDERR:\n\*\* \(seed:[0-9]+\): WARNING \*\*: Exception in class init closure\. Line 14 in .*\/gtype-class-init-exception\.js: ReferenceError Can't find variable: notAVariable

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

HelloWindowType = {
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
		prototype = notAVariable.notAProperty
    },
    init: function(klass)
    {
    }
}

try
{
    HelloWindow = new GType(HelloWindowType)
    w = new HelloWindow()
}
catch(e)
{
    testsuite.assert(e instanceof ReferenceError)
    // TODO: THIS TEST DOESN'T WORK EITHER!
    // what's going on with chained exception stuff
}

//testsuite.checkAsserts(1)

print("This test doesn't work yet...")
