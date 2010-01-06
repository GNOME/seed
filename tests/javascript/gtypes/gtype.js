#!../../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk

Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    class_init: function(klass, prototype)
    {
        prototype.message = "Prototypes!"
    },
    init: function(self)
    {
        this.title = "Hello!"
    }
})

w = new HelloWindow()

testsuite.assert(w.message = "Prototypes!")

w.signal.map.connect(
    function(widget)
    {
        testsuite.assert(widget.title == "Hello!")
    })

w.show()

testsuite.checkAsserts(2)
