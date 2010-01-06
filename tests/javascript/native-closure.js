#!../../src/seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

w = new Gtk.Window()
vbox = new Gtk.VBox()

closure = function(widget) {
    testsuite.assert(widget instanceof Gtk.Label || widget instanceof Gtk.Button)
}
w.add(vbox)

vbox.pack_start(new Gtk.Label())
vbox.pack_start(new Gtk.Button())

vbox.foreach(closure)

testsuite.checkAsserts(2)
