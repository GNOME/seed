#!/usr/bin/env seed

Seed.import_namespace("Clutter");
Seed.import_namespace("Gtk");
Seed.import_namespace("GtkClutter");
Seed.import_namespace("Pango");
Seed.import_namespace("PangoFT2");

var current_actor;
var font_list = new Array();

Gtk.init(null, null);
GtkClutter.init(null, null);

function get_font_list()
{
    context = new Pango.Context();
    description = Pango.FontDescription._new();
    description.set_family("");

    fontmap = new PangoFT2.FontMap();
    fontset = fontmap.load_fontset(context, 
			           description, 
			           Pango.language_get_default());
    fontset.foreach(
		function(fontset, font)
		{
		    font_list.push(font.describe().to_string().replace(" 0", ""));
		});
	
	font_list = font_list.sort();
}

function update_text(entry)
{
	current_actor.text = entry.text;
}

function update_font()
{
    current_actor.font_name = font_list[properties.font_combo.get_active()] + " " + parseFloat(properties.size_entry.text,10);
}

function add_actor()
{
    new pango_actor("Oh hi!", "DejaVu Serif 28");
}

function prop_editor()
{
	this.text = new Gtk.Entry();
	this.text.signal.changed.connect(update_text);
	
	this.new_button = new Gtk.ToolButton({stock_id:"gtk-add"});
	this.new_button.signal.clicked.connect(add_actor);
	
	this.font_combo = Gtk.ComboBox.new_text();
	this.font_combo.signal.changed.connect(update_font);
	for(var i in font_list)
    {
        this.font_combo.append_text(font_list[i]);
    }
    
    this.size_entry = new Gtk.Entry();
    this.size_entry.signal.activate.connect(update_font);
	
	this.hbox = new Gtk.HBox();
	this.hbox.pack_start(this.text, true, true);
	this.hbox.pack_start(this.font_combo, true, true);
	this.hbox.pack_start(this.size_entry);
	this.hbox.pack_start(this.new_button);
}

function clear_selected(stg, evt)
{
    if(stg.equals(stage.get_actor_at_pos(evt.get_x(), evt.get_y())))
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
    var pfd = Pango.font_description_from_string(actor.get_font_name());

    properties.size_entry.text = pfd.to_string().match(new RegExp("[0-9]+$"),"");
    properties.font_combo.set_active(font_list.indexOf(pfd.to_string().replace(new RegExp(" [0-9]+$"),"")));
}

function mouse_click(actor, evt)
{
	dx = evt.get_x() - actor.x;
	dy = evt.get_y() - actor.y;
	
	select_actor(actor);
	
	dragging = true;
	
	return true;
}

function mouse_release()
{
    dragging = false;
}

function mouse_moved(win, evt)
{
    if(!dragging)
        return;
    
    current_actor.x = evt.get_x() - dx;
    current_actor.y = evt.get_y() - dy;
}

function pango_actor(label, font)
{
    this.label = new Clutter.Label({text:label, font_name:font});
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
}

function ui_setup()
{
    var window = new Gtk.Window();
    window.signal.hide.connect(Gtk.main_quit);
    var gtkstage = new GtkClutter.Embed();

    properties = new prop_editor();
    var vbox = new Gtk.VBox();
    vbox.pack_start(gtkstage, true, true);
    vbox.pack_start(properties.hbox);

    window.resize(600,600);
    window.add(vbox);
    window.show_all();
    
    return gtkstage;
}

function pangotest_init()
{
    get_font_list();
    
    stage = ui_setup().get_stage();
    
    Clutter.set_motion_events_frequency(60);
    
    stage.signal.motion_event.connect(mouse_moved);
    stage.signal.button_release_event.connect(mouse_release);
    stage.signal.button_press_event.connect(clear_selected);
    
    pulser();
    
    create_default_actors();

    stage.show_all();

    Gtk.main();
}

function create_default_actors()
{
    select_actor((new pango_actor("Hello, world!", "DejaVu Sans 24")).label);
    new pango_actor("Oh hi!", "DejaVu Serif 28");
}

pangotest_init();
