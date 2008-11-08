#!/usr/bin/env seed
Seed.import_namespace("Gtk");
Gtk.init(null, null);

label = new Gtk.Label({label: "Hi!"});
button = new Gtk.Button({label: "Bye"});
entry = new Gtk.Entry();

packing = [  
	{child: label,
	 fill: false,
	 padding: 10,
	 position: Gtk.PackType.end},
	{child:button,
	 fill: false,
	 padding: 100,
	 expand: true},
	{child:entry}
	];

window = new Gtk.Window();
vbox = new Gtk.VBox();

vbox.pack(packing);

window.add(vbox);
window.show_all();
Gtk.main();