#!/usr/bin/env seed

imports.gi.versions.Clutter = "0.9";
imports.gi.versions.GtkClutter = "0.9";

Clutter = imports.gi.Clutter;
Gtk = imports.gi.Gtk;
GtkClutter = imports.gi.GtkClutter;
Pango = imports.gi.Pango;
PangoFT2 = imports.gi.PangoFT2;
GObject = imports.gi.GObject;

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
	}
});

PropertyEditor = new GType({
	parent: Gtk.HBox.type,
	name: "PropertyEditor",
	init: function()
	{
		// Private
		
		var loading = false;
		var text = new Gtk.Entry();
		var new_button = new Gtk.ToolButton({stock_id:"gtk-add"});
		var font_combo = new Gtk.ComboBox.text();
		var size_entry = new Gtk.Entry();
		
		var add_widget = function ()
		{
			stage.add_actor(new PangoWidget({text: "New Text...",
    								 font_name: "DejaVu Sans 24"}));
    		stage.show_all();
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
			loading = true;
			
			if(!actor)
			{
				text.text = "";
				size_entry.text = "";
				font_combo.set_active(font_list.indexOf("DejaVu Sans"));
				return;
			}
			
			text.text = actor.text;
			
			var pfd = Pango.Font.description_from_string(actor.get_font_name());

			size_entry.text = pfd.to_string().match(new RegExp("[0-9]+$"),"");
			font_combo.set_active(font_list.indexOf(pfd.to_string().replace(new RegExp(" [0-9]+$"),"")));
			
			loading = false;
		};
		
		this.commit_to_selected_actor = function ()
		{
			if(loading)
				return;
			
			selected_actor.text = text.text;
			selected_actor.font_name = font_list[font_combo.get_active()] + " " + parseFloat(size_entry.text,10);
		};
		
		text.signal.changed.connect(this.commit_to_selected_actor);
		new_button.signal.clicked.connect(add_widget);
		font_combo.signal.changed.connect(this.commit_to_selected_actor);
		size_entry.signal.activate.connect(this.commit_to_selected_actor);

		// Implementation
		
		populate_font_selector(font_combo);
		
		this.pack_start(text, true, true);
		this.pack_start(font_combo, true, true);
		this.pack_start(size_entry);
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
    stage.add_actor(new PangoWidget({text: "Hello, world!",
    								 font_name: "DejaVu Sans 24"}));
	stage.add_actor(new PangoWidget({text: "Oh hi!",
    								 font_name: "DejaVu Serif 32"}));
}

pangotest_init();
