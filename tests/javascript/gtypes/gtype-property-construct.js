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
                                 GObject.ParamFlags.CONSTRUCT | 
                                     GObject.ParamFlags.READABLE | 
                                     GObject.ParamFlags.WRITABLE))
    },
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
