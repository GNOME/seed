#!/usr/bin/env seed

Gtk = imports.gi.Gtk;

Gtk.init(Seed.argv);

var label = new Gtk.Label({label: "Hi!"});
var button = new Gtk.Button({label: "Bye"});
var entry = new Gtk.Entry();

var packing = [
    {
	child: label,
	fill: false,
	padding: 10,
	position: Gtk.PackType.END
    },
    {
	child: button,
	fill: false,
	padding: 100,
	expand: true
    },
    {
	child:entry
    }
];

window = new Gtk.Window();
window.signal.hide.connect(Gtk.main_quit);
vbox = new Gtk.VBox();

vbox.pack(packing);

window.add(vbox);
window.show_all();
Gtk.main();
