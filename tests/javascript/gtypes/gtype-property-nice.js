#!../../../src/seed

testsuite = imports.testsuite
GObject = imports.gi.GObject
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    properties: [{
        name: "randomproperty",
        type: GObject.TYPE_BOOLEAN,
        nick: "randomproperty",
        blurb: "longer blurb about property",
        default_value: true,
        flags: (GObject.ParamFlags.CONSTRUCT | GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE)}],
    init: function(klass)
    {

    }
})

w = new HelloWindow()

testsuite.assert(w.randomproperty == 1)
w.randomproperty = 5
testsuite.assert(w.randomproperty == 1)
w.randomproperty = 0
testsuite.assert(w.randomproperty == 0)
