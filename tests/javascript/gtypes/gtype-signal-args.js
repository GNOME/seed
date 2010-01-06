#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
GObject = imports.gi.GObject
Gtk.init(Seed.argv)

HelloWindow = new GType({
    parent: Gtk.Window.type,
    name: "HelloWindow",
    signals: [{name: "hello",
               parameters: [GObject.TYPE_INT,
                            GObject.TYPE_STRING],
               return_type: Gtk.Window.type}]
})

w = new HelloWindow()

w.signal.hello.connect(function(object, number, string)
{
   var win = new Gtk.Window()
   testsuite.assert(number == 2)
   testsuite.assert(string == "Test")
   return win
})

testsuite.assert(w.signal.hello.emit(2, "Test") instanceof Gtk.Window)

testsuite.checkAsserts(3)
