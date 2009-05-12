#!/usr/bin/env seed

imports.gi.versions.Clutter = "0.9";
imports.gi.versions.GtkClutter = "0.9";

Clutter = imports.gi.Clutter;
Gtk = imports.gi.Gtk;
GtkClutter = imports.gi.GtkClutter;
Pango = imports.gi.Pango;
PangoFT2 = imports.gi.PangoFT2;

Gtk.init(Seed.argv);
GtkClutter.init(Seed.argv);

PangoWidget = new GType({
	parent: Clutter.Text.type,
	name: "PangoWidget",
	init: function()
	{
		// Private
		
		var widget_clicked = function (actor, event)
		{
			Seed.print(actor.text);
			actor.set_selected(true);
			return false;
		};
		
		// Public
		
		this.set_selected = function (selected)
		{
			Seed.print(this);
			if(selected)
			{
				this.opacity = 0.0;
			}
			else
			{
				this.opacity = 255.0;
			}
		};
		
		// Implementation
		
		this.text = "Hello, world!";
		this.reactive = true;
		this.signal.button_press_event.connect(widget_clicked);
	}
});

ContainerWidget = new GType({
	parent: GtkClutter.Embed.type,
	name: "ContainerWidget",
	init: function()
	{
		
	}
});

/*FontSelector = new GType({
	parent: Gtk.ComboBox.type,
	name: "FontSelector",
	init: function()
	{
		var font_list = [];
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
		    this.append_text(font_list[i]);
		}
	}
});*/

PropertyEditor = new GType({
	parent: Gtk.HBox.type,
	name: "PropertyEditor",
	init: function()
	{
		// Private
		var text = new Gtk.Entry();
		var new_button = new Gtk.ToolButton({stock_id:"gtk-add"});
		//var font_combo = new FontSelector.text();
		var size_entry = new Gtk.Entry();
		
		var add_widget = function ()
		{
			stage.add_actor(new PangoWidget({text: "New Text...",
    								 font_name: "DejaVu Sans 24"}));
    		stage.show_all();
		};
		
		//text.signal.changed.connect(update_text);
		new_button.signal.clicked.connect(add_widget);
		//font_combo.signal.changed.connect(update_font);
		//size_entry.signal.activate.connect(update_font);

		// Implementation
		this.pack_start(text, true, true);
		//this.pack_start(font_combo, true, true);
		this.pack_start(size_entry);
		this.pack_start(new_button);
	}
});

function update_font()
{
    //current_actor.font_name = font_list[properties.font_combo.get_active()] + " " + parseFloat(properties.size_entry.text,10);
}

/*function clear_selected(stg, evt)
{
    if(stg.equals(stage.get_actor_at_pos(evt.x, evt.y)))
        select_actor(null);
}

function select_actor(actor)
{
    timeline.rewind();
    
    if(current_actor)
        current_actor.opacity = 255;
    
    current_actor = actor;
    
    if(!current_actor)
    {
        // defaults
        properties.text.text = "";
        properties.font_combo.set_active(font_list.indexOf("DejaVu Sans"));
        return;
    }
    
    properties.text.text = actor.text;
    var pfd = Pango.Font.description_from_string(actor.get_font_name());

    properties.size_entry.text = pfd.to_string().match(new RegExp("[0-9]+$"),"");
    properties.font_combo.set_active(font_list.indexOf(pfd.to_string().replace(new RegExp(" [0-9]+$"),"")));
}

function mouse_click(actor, evt)
{
	dx = evt.mouse.x - actor.x;
	dy = evt.mouse.y - actor.y;
	
	select_actor(actor);

	dragging = true;
	
	return true;
}

function mouse_release()
{
    dragging = false;
    
    return false;
}

function mouse_moved(win, evt)
{
    if(!dragging)
        return false;
    
    current_actor.x = evt.mouse.x - dx;
    current_actor.y = evt.mouse.y - dy;
    
    return false;
}

function pango_actor(label, font)
{
    this.label = new Clutter.Text({text:label, font_name:font});
    this.label.reactive = true;
    this.label.signal.button_press_event.connect(mouse_click);
    
    stage.add_actor(this.label);
}

function pulser()
{
    timeline = new Clutter.Timeline({fps:30, num_frames:628, loop:true});

    timeline.signal.new_frame.connect(function(timeline, frame_num)
	{ current_actor.opacity = ((Math.sin(frame_num/7)+1) * 67) + 120; });

    timeline.start();
}*/

function ui_setup()
{
    var window = new Gtk.Window();
    window.signal.hide.connect(Gtk.main_quit);
    
    var gtkstage = new ContainerWidget();

    properties = new PropertyEditor();
    
    var vbox = new Gtk.VBox();
    vbox.pack_start(gtkstage, true, true);
    vbox.pack_start(properties);

    window.resize(600,600);
    window.add(vbox);
    window.show_all();
    
    return gtkstage;
}

function pangotest_init()
{
    stage = ui_setup().get_stage();
    
    Clutter.set_motion_events_frequency(60);
    
    //stage.signal.motion_event.connect(mouse_moved);
    //stage.signal.button_release_event.connect(mouse_release);
    //stage.signal.button_press_event.connect(clear_selected);
    
    //pulser();
    
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
