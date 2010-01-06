#!../../src/seed

testsuite = imports.testsuite;
Gtk = imports.gi.Gtk;

Gtk.init(Seed.argv);

w = new Gtk.Window();

// Closure will always be GCed at end of signal.
signal = function()
{
    widgets = []
	w.foreach(function(widget){widgets.push(widget.toString())})
	testsuite.assert(widgets[0] == ["[object GtkVBox]"])
	testsuite.assert(widgets.length == 1)
};

vbox = new Gtk.VBox();

w.add(vbox);

w.signal.show.connect(signal);

w.show_all();

testsuite.checkAsserts(2)
