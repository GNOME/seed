#!../../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
GObject = imports.gi.GObject
Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
        klass.c_install_property(GObject.param_spec_boolean(
            "test",
            "test property",
            "A test property!",
            false,
            GObject.ParamFlags.READABLE | 
                GObject.ParamFlags.WRITABLE))
    },
    init: function()
    {
    }
})

w = new HelloWindow({test: true})
testsuite.assert(w.test == true)
w = new HelloWindow()
testsuite.assert(w.test == false)

