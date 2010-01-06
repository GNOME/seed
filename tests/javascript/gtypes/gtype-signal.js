#!/usr/bin/env seed

testsuite = imports.testsuite
Gtk = imports.gi.Gtk
Gtk.init(Seed.argv)

HelloWindowType = {
    parent: Gtk.Window.type,
    name: "HelloWindow",
    signals: [{name: "hello"}, {name: "goodbye"}]
}

HelloWindow = new GType(HelloWindowType)
w = new HelloWindow()

w.signal.hello.connect(function(){hello = 5})
w.signal.goodbye.connect(function(){goodbye = 10})

w.signal.hello.emit()
w.signal.goodbye.emit()

testsuite.assert(hello == 5)
testsuite.assert(goodbye == 10)
