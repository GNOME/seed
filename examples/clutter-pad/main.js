#!/usr/bin/env seed

Clutter = imports.gi.Clutter;
Gtk = imports.gi.Gtk;
GtkSource = imports.gi.GtkSource;
GtkClutter = imports.gi.GtkClutter;
sandbox = imports.sandbox;
Gio = imports.gi.Gio;
Pango = imports.gi.Pango;

Gtk.init(Seed.argv);
GtkClutter.init(Seed.argv);

var context;

function reset_stage()
{
	stage.remove_all();
	
	stage.color = {alpha: 255};
	stage.show_cursor();
}

function load_file(filename)
{
	new_file();
	
	current_filename = filename;
	window.title = "ClutterPad - " + filename;
	
	file = Gio.file_new_for_path(filename);
	source_buf.text = file.read().get_contents();
	
	execute_file();
}

function new_file(button)
{
	reset_stage();
	
	if(button)
		ui.get_object("example_selector").set_active(-1);
	
	window.title = "ClutterPad";
	current_filename = "";
	source_buf.text = "GObject = imports.gi.GObject;\nClutter = imports.gi.Clutter;\nstage = Clutter.Stage.get_default();\n";
}

function open_file()
{
	file_chooser = ui.get_object("open_file_dialog");
	file_chooser.set_filter(js_file_filter);
	
	if(file_chooser.run() > 0)
	{
		load_file(file_chooser.get_filename());
	}
	
	file_chooser.hide();
}

function save_file(filename)
{
	if(current_filename == "")
	{
		file_chooser = ui.get_object("save_file_dialog");
		file_chooser.set_filter(js_file_filter);

		if(file_chooser.run() > 0)
		{
			current_filename = file_chooser.get_filename();
			window.title = "ClutterPad - " + current_filename;
		}
		
		file_chooser.hide();
	}

	if(current_filename != "")
	{
		try
		{
			Gio.simple_write(current_filename, source_buf.text);
		}
		catch(e)
		{
			print(e.message);
		}
	}
}

function populate_example_selector(selector)
{
	// Since we're using GtkBuilder, we can't make a Gtk.ComboBox.text. Instead,
	// we'll construct the cell renderer here, once, and use that.
	var cell = new Gtk.CellRendererText();
	selector.pack_start(cell, true);
	selector.add_attribute(cell, "text", 0);

	file = Gio.file_new_for_path("examples");
	enumerator = file.enumerate_children("standard::name");

	while((child = enumerator.next_file()))
	{
		var fname = child.get_name();
		
		if(fname.match(/\.js$/))
			selector.append_text(fname);
	}
}

function select_example(selector, ud)
{
	load_file("examples/" + selector.get_active_text());
}

function execute_file(button)
{
    try
    {
		error_buf.text = '';
		var a = new Gtk.TextIter(); var b = new Gtk.TextIter();
		source_buf.get_selection_bounds(a, b);
		var slice = source_buf.get_slice(a, b);
		if (slice == '')
		{
			reset_stage();
			if (context)
				context.destroy();
			context = new sandbox.Context();
			context.add_globals();
			context.eval(source_buf.text)
		}
		else
		{
			context.eval(slice);
		}
    }
    catch (e)
    {
		print(e.message);
		error_buf.text = e.message;
    }
};

function load_accelerator(action, accel)
{
	action.set_accel_group(accels);
	actions.add_action_with_accel(action, accel);
	action.connect_accelerator();
}

var current_filename = "";
var stage_manager = Clutter.StageManager.get_default();
var js_file_filter = new Gtk.FileFilter();
js_file_filter.add_mime_type("text/javascript");
var source_lang_mgr = new GtkSource.SourceLanguageManager();
var js_lang = source_lang_mgr.get_language("js");

var ui = new Gtk.Builder();
ui.add_from_file("clutter-pad.ui");

var actions = new Gtk.ActionGroup({name: "toolbar"});
var accels = new Gtk.AccelGroup();
var window = ui.get_object("window");
window.signal.hide.connect(Gtk.main_quit);
window.add_accel_group(accels);

var clutter = ui.get_object("clutter");
var stage = clutter.get_stage();
stage_manager.set_default_stage(stage);

var error_buf = ui.get_object("error_view").get_buffer();
var source_buf = new GtkSource.SourceBuffer({language: js_lang});

populate_example_selector(ui.get_object("example_selector"));

load_accelerator(ui.get_object("new_action"), null);
load_accelerator(ui.get_object("open_action"), null);
load_accelerator(ui.get_object("save_action"), null);
load_accelerator(ui.get_object("execute_action"), "<Ctrl>r");

ui.get_object("new_action").signal.activate.connect(new_file);
ui.get_object("open_action").signal.activate.connect(open_file);
ui.get_object("save_action").signal.activate.connect(save_file);
ui.get_object("example_selector").signal.changed.connect(select_example);
ui.get_object("execute_action").signal.activate.connect(execute_file);

ui.get_object("source_view").set_buffer(source_buf);
ui.get_object("source_view").modify_font(Pango.Font.description_from_string("monospace 10"));

window.resize(800, 600);
window.show_all();

new_file();

Gtk.main();

