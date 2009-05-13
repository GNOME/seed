#!/usr/bin/env seed

imports.gi.versions.Clutter = "0.9";
imports.gi.versions.GtkClutter = "0.9";

Clutter = imports.gi.Clutter;
Gtk = imports.gi.Gtk;
GtkClutter = imports.gi.GtkClutter;
Pango = imports.gi.Pango;
PangoFT2 = imports.gi.PangoFT2;
GObject = imports.gi.GObject;
Gdk = imports.gi.Gdk;

Gtk.init(Seed.argv);
GtkClutter.init(Seed.argv);

var font_list = [];
selected_actor = null;

PangoWidget = new GType({
	parent: Clutter.Text.type,
	name: "PangoWidget",
	init: function()
	{
		// Private
		
		var dx, dy, dragging;
		
		var widget_clicked = function (actor, event)
		{
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
		
		var widget_dragged = function (actor, event)
		{
			if(!dragging)
				return false;
		
			actor.x = event.motion.x - dx;
			actor.y = event.motion.y - dy;
		
			return false;
		};
		
		var widget_unclicked = function ()
		{
			dragging = false;
			
			return false;
		};
		
		// Public
		
		this.set_selected = function (selected)
		{
			this.editable = false;
			
			if(selected)
			{
				var children = stage.get_children();
				
				for(var id in children)
				{
					children[id].set_selected(false);
				}
				
				this.timeline = new Clutter.Timeline({fps:30, num_frames:628, loop:true});
				this.timeline.signal.new_frame.connect(function(timeline, frame_num, ud)
				{
					ud.opacity = ((Math.sin(frame_num/7)+1) * 67) + 120;
				}, this);
			
				this.timeline.start();
				
				stage.raise_child(this, null);
				
				selected_actor = this;
				properties.load_from_actor(this);
			}
			else
			{
				if(this.timeline)
				{
					this.timeline.stop();
					this.opacity = 255;
				}
			}
		};
		
		// Implementation
		
		this.text = "Hello, world!";
		this.reactive = true;
		this.signal.button_press_event.connect(widget_clicked);
		this.signal.button_release_event.connect(widget_unclicked);
		this.signal.motion_event.connect(widget_dragged);
		this.color = {red: 0, green: 0, blue: 0, alpha: 255};
	}
});

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
		
		var add_widget = function ()
		{
			stage.add_actor(new PangoWidget({text: "New Text...",
    								 font_name: "DejaVu Sans 24"}));
    		stage.show_all();
		};
		
		var clear_widgets = function ()
		{
			var children = stage.get_children();
				
			for(var id in children)
				stage.remove_actor(children[id]);
		};
		
		var delete_widget = function ()
		{
			stage.remove_actor(selected_actor);
			selected_actor = null;
			properties.load_from_actor(null);
		};
		
		var populate_font_selector = function (combo_box)
		{
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
			{
				combo_box.append_text(font_list[i]);
			}
		}
		
		// Public
		
		this.load_from_actor = function (actor)
		{
			if(!actor)
			{
				delete_button.sensitive = size_scale.sensitive =
					font_combo.sensitive = color_button.sensitive = false;
			
			
				size_scale.set_value(8);
				font_combo.set_active(font_list.indexOf("DejaVu Sans"));
				color_button.color = {red: 0, green: 0, blue: 0};
				
				return;
			}
			
			delete_button.sensitive = size_scale.sensitive =
				font_combo.sensitive = color_button.sensitive = true;
			
			loading = true;
			
			var aclr = new Clutter.Color();
			actor.get_color(aclr);
			
			var clr = new Gdk.Color();
			clr.red = (aclr.red / 255) * 65535;
			clr.green = (aclr.green / 255) * 65535;
			clr.blue = (aclr.blue / 255) * 65535;
			
			color_button.set_color(clr);
			
			var pfd = Pango.Font.description_from_string(actor.get_font_name());
			size_scale.set_value(parseFloat(pfd.to_string().match(new RegExp("[0-9\.]+$"),""), 10));
			font_combo.set_active(font_list.indexOf(pfd.to_string().replace(new RegExp(" [0-9\.]+$"),"")));
			
			loading = false;
		};
		
		this.commit_to_selected_actor = function ()
		{
			if(loading || !selected_actor)
				return;
			
			selected_actor.font_name = font_list[font_combo.get_active()] + " " + size_scale.get_value();
			
			var clr = new Gdk.Color();
			color_button.get_color(clr);

			selected_actor.color = {red: (255*clr.red) / 65535,
									green: (255*clr.green) / 65535,
									blue: (255*clr.blue) / 65535,
									alpha: 255};
		};
		
		new_button.signal.clicked.connect(add_widget);
		delete_button.signal.clicked.connect(delete_widget);
		clear_button.signal.clicked.connect(clear_widgets);
		font_combo.signal.changed.connect(this.commit_to_selected_actor);
		size_scale.signal.value_changed.connect(this.commit_to_selected_actor);
		color_button.signal.color_set.connect(this.commit_to_selected_actor);

		// Implementation
		
		size_scale.adjustment.lower = 8;
		size_scale.adjustment.upper = 200;
		size_scale.adjustment.step_increment = 1;
		size_scale.set_digits(0);
		size_scale.set_draw_value(false);
		
		populate_font_selector(font_combo);
		this.load_from_actor(null);
		
		this.pack_start(color_button);
		this.pack_start(font_combo);
		this.pack_start(size_scale, true, true);
		this.pack_start(delete_button);
		this.pack_start(clear_button);
		
		this.pack_start(new_button);
	}
});

function clear_selected(stg, evt)
{
    if(stg.equals(stage.get_actor_at_pos(evt.button.x, evt.button.y)))
    {
    	var children = stage.get_children();
				
		for(var id in children)
		{
			children[id].set_selected(false);
		}
	}
	
	selected_actor = null;
	properties.load_from_actor(null);
	
	return false;
}

function ui_setup()
{
    var window = new Gtk.Window();
    window.signal.hide.connect(Gtk.main_quit);
    
    var gtkstage = new GtkClutter.Embed();

    properties = new PropertyEditor();
    
    var vbox = new Gtk.VBox();
    vbox.pack_start(gtkstage, true, true);
    vbox.pack_start(properties);

    window.resize(600, 600);
    window.add(vbox);
    window.show_all();
    
    return gtkstage;
}

function pangotest_init()
{
    stage = ui_setup().get_stage();
    
    stage.signal.button_press_event.connect(clear_selected);
    
    Clutter.set_motion_events_frequency(60);
    
    create_default_actors();

    stage.show_all();

    Gtk.main();
}

function create_default_actors()
{
    stage.add_actor(new PangoWidget({text: "Welcome to the wild world of Seed+Clutter+Pango!",
    								 font_name: "DejaVu Sans 16",
    								 x: 20, y: 200}));
	stage.add_actor(new PangoWidget({text: "Each text element is draggable,",
    								 font_name: "DejaVu Sans 12",
    								 x: 100, y: 230}));
    stage.add_actor(new PangoWidget({text: "resizable,",
    								 font_name: "DejaVu Sans 48",
    								 x: 200, y: 245}));
    stage.add_actor(new PangoWidget({text: "editable, (double click me to edit!)",
    								 font_name: "DejaVu Serif 14",
    								 x: 150, y: 315}));
    stage.add_actor(new PangoWidget({text: "styleable,",
    								 font_name: "FreeMono 24",
    								 x: 220, y: 335}));
    
    stage.add_actor(new PangoWidget({text: "and",
    								 font_name: "DejaVu Sans 20",
    								 x: 100, y: 370,
    								 color: {red: 255, alpha: 255}}));
    stage.add_actor(new PangoWidget({text: "even",
    								 font_name: "DejaVu Sans 20",
    								 x: 160, y: 370,
    								 color: {blue: 255, alpha: 255}}));
    stage.add_actor(new PangoWidget({text: "colorable!",
    								 font_name: "DejaVu Sans 20",
    								 x: 235, y: 370,
    								 color: {green: 255, alpha:255}}));
}

pangotest_init();
