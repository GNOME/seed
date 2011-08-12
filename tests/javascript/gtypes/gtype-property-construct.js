#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
GObject = imports.gi.GObject
Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    properties: [
        {
            name: "test",
            type: GObject.TYPE_BOOLEAN,
            nick: "test property",
            blurb: "A test property!",
            default_value: false,
            flags: (GObject.ParamFlags.CONSTRUCT | GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE)
        }
    ],
   
    init: function()
    {
        testsuite.assert(this.test == true)
    }
})

w = new HelloWindow({test: true})

testsuite.assert(w.test == true)

try
{
    w.test = "arst"
}
catch(e)
{
    testsuite.assert(e.name == "ConversionError")
}

testsuite.checkAsserts(3)
