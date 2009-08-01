#!/usr/bin/env seed

imports.gi.versions.Clutter = "1.0";
imports.gi.versions.GtkClutter = "1.0";

Clutter = imports.gi.Clutter;
Gtk = imports.gi.Gtk;
GtkClutter = imports.gi.GtkClutter;
Pango = imports.gi.Pango;
PangoFT2 = imports.gi.PangoFT2;
GObject = imports.gi.GObject;
Gdk = imports.gi.Gdk;
Gio = imports.gi.Gio;
GLib = imports.gi.GLib;

JSON = imports.JSON;

Gtk.init(Seed.argv);
GtkClutter.init(Seed.argv);

var filename = GLib.build_filenamev([GLib.get_home_dir(),".pangojsrc"]);
var font_list = [];
selected_actor = null;

// The PangoActor Clutter widget provides a draggable, in-place editable
// text widget which provides evidence of its selection by pulsing.
PangoActor = new GType({
	parent: Clutter.Text.type,
	name: "PangoActor",
	init: function()
	{
		// Private

		var dx, dy, dragging;

		var actor_clicked = function (actor, event)
		{
			// Steal keyboard focus
			gtkstage.has_focus = true;

			// A double click allows the user to edit the text
			if(event.button.click_count > 1)
			{
				actor.editable = true;
				return true;
			}

			dx = event.button.x - actor.x;
			dy = event.button.y - actor.y;
			dragging = true;

			actor.set_selected(true);

			return false;
		};

		var actor_dragged = function (actor, event)
		{
			if(!dragging)
				return false;

			actor.x = event.motion.x - dx;
			actor.y = event.motion.y - dy;

			return false;
		};

		var actor_unclicked = function ()
		{
			dragging = false;

			return false;
		};

		// Public

		this.set_selected = function (selected)
		{
			if(selected)
			{
				var children = stage.get_children();

				// Deselect all actors (except ourself)
				for(var id in children)
					if(children[id] != this)
						children[id].set_selected(false);
					else
					{
						if(this.timeline)
							this.timeline.stop();
						this.opacity = 255;
					}

				// Start a pulsing effect to indicate that this actor is selected
				this.timeline = new Clutter.Timeline({fps:30, num_frames:628, loop:true});
				this.timeline.signal.new_frame.connect(function(timeline, frame_num, ud)
				{
					ud.opacity = ((Math.sin(frame_num/7)+1) * 67) + 120;
				}, this);

				this.timeline.start();

				// Raise selected actor to the top in z-order
				stage.raise_child(this, null);

				selected_actor = this;
				properties.load_from_actor(this);
			}
			else
			{
				if(this.timeline)
				{
					// Stop the pulsing effect
					this.timeline.stop();
					this.opacity = 255;
					this.editable = false;
				}
			}
		};

		this.serialize = function ()
		{
			var serobj = {};
			serobj.text = this.text;
			serobj.x = this.x;
			serobj.y = this.y;
			serobj.font_name = this.font_name;

			//var icolor = this.get_color();

			/*serobj.color = {red: icolor.red + 0,
							green: icolor.green + 0,
							blue: icolor.blue + 0,
							alpha: 255};*/

			//print(this.color.red);

			return serobj;
		}

		// Implementation

		this.text = "Hello, world!";
		this.reactive = true;
		this.signal.button_press_event.connect(actor_clicked);
		this.signal.button_release_event.connect(actor_unclicked);
		this.signal.motion_event.connect(actor_dragged);
		this.color = {red: 0, green: 0, blue: 0, alpha: 255};
	}
});

// The PropertyEditor Gtk widget provides adjustments for various properties
// related to the currently selected PangoActor.
PropertyEditor = new GType({
	parent: Gtk.HBox.type,
	name: "PropertyEditor",
	init: function()
	{
		// Private

		var loading = false;

		var new_button = new Gtk.ToolButton({stock_id:"gtk-add"});
		var delete_button = new Gtk.ToolButton({stock_id:"gtk-delete"});
		var clear_button = new Gtk.ToolButton({stock_id:"gtk-clear"});

		var font_combo = new Gtk.ComboBox.text();
		var size_scale = new Gtk.HScale();
		var color_button = new Gtk.ColorButton();

		var add_actor = function ()
		{
			stage.add_actor(new PangoActor({text: "Play with me!",
    								 font_name: "DejaVu Sans 24"}));
    		stage.show_all();
		};

		var clear_actors = function ()
		{
			var children = stage.get_children();

			for(var id in children)
				stage.remove_actor(children[id]);
		};

		var delete_actor = function ()
		{
			stage.remove_actor(selected_actor);
			selected_actor = null;
			properties.load_from_actor(null);
		};

		var populate_font_selector = function (combo_box)
		{
			// List all of the fonts available to Pango, and populate the
			// font selection combo box with their names.
			var context = new Pango.Context();
			var description = new Pango.FontDescription.c_new();
			description.set_family("");

			var fontmap = new PangoFT2.FontMap();
			var fontset = fontmap.load_fontset(context,
							   				   description,
							   				   Pango.language_get_default());

			fontset.foreach(function(fontset, font)
			{
				font_list.push(font.describe().to_string().replace(" 0", ""));
			});

			font_list = font_list.sort();

			for(var i in font_list)
				combo_box.append_text(font_list[i]);
		}

		// Public

		this.load_from_actor = function (actor)
		{
			// If there is no selection, reset everything to defaults and
			// prevent the user from editing the properties.
			if(!actor)
			{
				delete_button.sensitive = size_scale.sensitive =
					font_combo.sensitive = color_button.sensitive = false;


				size_scale.set_value(0);
				font_combo.set_active(-1);
				color_button.color = {red: 0, green: 0, blue: 0};

				return;
			}

			// Make all of the properties editable
			delete_button.sensitive = size_scale.sensitive =
				font_combo.sensitive = color_button.sensitive = true;

			loading = true;

			// Convert the actor's ClutterColor to a GdkColor
			var aclr = new Clutter.Color();
			actor.get_color(aclr);
			color_button.set_color({red: (aclr.red / 255) * 65535,
									green: (aclr.green / 255) * 65535,
									blue: (aclr.blue / 255) * 65535});

			// Parse and display the actor's font face and size
			var pfd = Pango.Font.description_from_string(actor.get_font_name());
			size_scale.set_value(parseFloat(pfd.to_string().match(new RegExp("[0-9\.]+$"),""), 10));
			font_combo.set_active(font_list.indexOf(pfd.to_string().replace(new RegExp(" [0-9\.]+$"),"")));

			loading = false;
		};

		this.commit_to_selected_actor = function ()
		{
			if(loading || !selected_actor)
				return;

			// Combine the selected font face and size, apply them to the actor
			selected_actor.font_name = font_list[font_combo.get_active()] +
									   " " + size_scale.get_value();

			// Convert ColorButton's GdkColor to a ClutterColor for the actor
			var clr = new Gdk.Color();
			color_button.get_color(clr);
			selected_actor.color = {red: (255*clr.red) / 65535,
									green: (255*clr.green) / 65535,
									blue: (255*clr.blue) / 65535,
									alpha: 255};
		};

		// Implementation

		populate_font_selector(font_combo);

		new_button.signal.clicked.connect(add_actor);
		delete_button.signal.clicked.connect(delete_actor);
		clear_button.signal.clicked.connect(clear_actors);
		font_combo.signal.changed.connect(this.commit_to_selected_actor);
		size_scale.signal.value_changed.connect(this.commit_to_selected_actor);
		color_button.signal.color_set.connect(this.commit_to_selected_actor);

		size_scale.adjustment.lower = 8;
		size_scale.adjustment.upper = 200;
		size_scale.adjustment.step_increment = 1;
		size_scale.set_digits(0);
		size_scale.set_draw_value(false);

		this.pack_start(color_button);
		this.pack_start(font_combo);
		this.pack_start(size_scale, true, true);
		this.pack_start(delete_button);
		this.pack_start(clear_button);
		this.pack_start(new_button);

		// Initially display default values and prevent editing of properties
		this.load_from_actor(null);
	}
});

function clear_selected(actor, event)
{
	// If the stage is clicked, deselect all actors

	var children = stage.get_children();

	for(var id in children)
		children[id].set_selected(false);

	selected_actor = null;
	properties.load_from_actor(null);

	return false;
}

function create_default_actors()
{
	// Add a few initial actors to explain the operation of the program

    stage.add_actor(new PangoActor({text: "Welcome to the wild world of Seed+Clutter+Pango!",
    								 font_name: "DejaVu Sans 16",
    								 x: 20, y: 200}));
	stage.add_actor(new PangoActor({text: "Each text element is draggable,",
    								 font_name: "DejaVu Sans 12",
    								 x: 100, y: 230}));
    stage.add_actor(new PangoActor({text: "resizable,",
    								 font_name: "DejaVu Sans 48",
    								 x: 200, y: 245}));
    stage.add_actor(new PangoActor({text: "editable, (double click me to edit!)",
    								 font_name: "DejaVu Serif 14",
    								 x: 150, y: 315}));
    stage.add_actor(new PangoActor({text: "styleable,",
    								 font_name: "FreeMono 24",
    								 x: 220, y: 335}));

    stage.add_actor(new PangoActor({text: "and",
    								 font_name: "DejaVu Sans 20",
    								 x: 100, y: 370,
    								 color: {red: 255, alpha: 255}}));
    stage.add_actor(new PangoActor({text: "even",
    								 font_name: "DejaVu Sans 20",
    								 x: 160, y: 370,
    								 color: {blue: 255, alpha: 255}}));
    stage.add_actor(new PangoActor({text: "colorable!",
    								 font_name: "DejaVu Sans 20",
    								 x: 235, y: 370,
    								 color: {green: 255, alpha:255}}));
}

function save_and_quit()
{
	var output = [];
	var children = stage.get_children();

	for(var id in children)
		output.push(children[id].serialize());

	Gio.simple_write(filename, JSON.stringify(output));

	Gtk.main_quit();
}

function load_actors()
{
	try
	{
		var file = Gio.file_new_for_path(filename);
		var input = JSON.parse(file.read().get_contents());

		for(var id in input)
			stage.add_actor(new PangoActor(input[id]));
	}
	catch (e)
	{
		create_default_actors();
	}
}

var window = new Gtk.Window();
window.signal.hide.connect(save_and_quit);

var gtkstage = new GtkClutter.Embed();
var stage = gtkstage.get_stage();
stage.signal.button_press_event.connect(clear_selected);

var properties = new PropertyEditor();

var vbox = new Gtk.VBox();
vbox.pack_start(gtkstage, true, true);
vbox.pack_start(properties);

window.resize(600, 600);
window.add(vbox);
window.show_all();

load_actors();
stage.show_all();

Gtk.main();
