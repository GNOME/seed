#!/usr/bin/env seed

Seed.import_namespace("Gtk");

Gtk.init(null, null);

var label = new Gtk.Label({label: "Hi!"});
var button = new Gtk.Button({label: "Bye"});
var entry = new Gtk.Entry();

var packing = [
	{
		child: label,
		fill: false,
		padding: 10,
		position: Gtk.PackType.End
	},
	{
		child:button,
		fill: false,
		padding: 100,
		expand: true
	},
	{
		child:entry
	}
	];

window = new Gtk.Window();
window.signal.hide.connect(function () { Gtk.main_quit(); });
vbox = new Gtk.VBox();

vbox.pack(packing);

window.add(vbox);
window.show_all();
Gtk.main();
