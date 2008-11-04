#!/usr/local/bin/seed

Seed.import_namespace("Clutter");
Seed.import_namespace("Gtk");
Seed.import_namespace("GtkClutter");

Gtk.init(null, null);
GtkClutter.init(null, null);

var current_text;

function update_text(entry)
{
	current_text.text = entry.text;
}

function prop_editor()
{
	this.text = new Gtk.Entry();
	this.text.signal_changed.connect(update_text);
	
	this.hbox = new Gtk.HBox();
	this.hbox.pack_start(this.text, true, true);
}

var window = new Gtk.Window();
var gtkstage = new GtkClutter.Embed();

var properties = new prop_editor();
var vbox = new Gtk.VBox();
vbox.pack_start(gtkstage, true, true);
vbox.pack_start(properties.hbox);

window.resize(600,600);
window.add(vbox);
window.show_all();




function mouse_click(win, evt)
{
	Seed.print(evt);
	Seed.print(evt.get_stage);
}

var stage = gtkstage.get_stage();
stage.show_all();

stage.signal_button_press_event.connect(mouse_click);

current_text = new Clutter.Label({text:"Hello, world!",
			     font_name:"Bitstream Vera Sans 24"});

stage.add_actor(current_text);

Gtk.main();