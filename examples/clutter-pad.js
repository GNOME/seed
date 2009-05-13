#!/usr/bin/env seed

imports.gi.versions.Clutter = "0.9";
imports.gi.versions.GtkClutter = "0.9";

Clutter = imports.gi.Clutter;
Gtk = imports.gi.Gtk;
GtkSource = imports.gi.GtkSource;
GtkClutter = imports.gi.GtkClutter;
GtkSource = imports.gi.GtkSource;
GLib = imports.gi.GLib;
sandbox = imports.sandbox;
Gio = imports.gi.Gio;

Gtk.init(Seed.argv);
GtkClutter.init(Seed.argv);

var current_filename = "";
var stage, gtkstage = null;
var stage_manager = Clutter.StageManager.get_default();

function evaluate(button)
{
    try
    {
		error_buf.text = '';
		var a = new Gtk.TextIter(); var b = new Gtk.TextIter();
		source_buf.get_selection_bounds(a, b);
		var slice = source_buf.get_slice(a, b);
		if (slice == '')
		{
			context.destroy();
			context = new sandbox.Context();
			context.add_globals();
			context.eval(source_buf.text)
		}
		else
		{
			context.eval(slice)
		}
    }
    catch (e)
    {
		error_buf.text = e.message;
    }
};

function populate_example_selector()
{
	file = Gio.file_new_for_path("clutter-pad-examples");
	enumerator = file.enumerate_children("standard::name");

	while((child = enumerator.next_file()))
	{
		var fname = child.get_name();
		if(fname.match(/\.js$/))
			example_selector.append_text(child.get_name());
	}
}

function new_file()
{
	current_filename = "";
	
	// TODO: cleanse the stage (or make a new one!) each time around...
	
	//if(gtkstage)
		//pane.remove(gtkstage);
	
	//gtkstage = new GtkClutter.Embed();
	//stage = gtkstage.get_stage();
	
	pane.add1(gtkstage);
	
	//stage_manager.set_default_stage(stage);
	
	//gtkstage.show_all();
	//stage.show_all();
}

function open_file()
{
	var file_chooser = new Gtk.FileChooserDialog();
	var file_filter = new Gtk.FileFilter();
	
	file_filter.add_mime_type("text/javascript");
	file_chooser.set_filter(file_filter);
	file_chooser.add_button("Cancel", Gtk.ResponseType.CANCEL);
	file_chooser.add_button("Open", Gtk.ResponseType.ACCEPT);
	file_chooser.set_action(Gtk.FileChooserAction.OPEN);

	if(file_chooser.run() == Gtk.ResponseType.ACCEPT)
	{
		load_file(file_chooser.get_filename());
	}

	file_chooser.destroy();
}

function load_file(filename)
{
	new_file();
	
	current_filename = filename;
	window.title = "ClutterPad - " + filename;
	
	file = Gio.file_new_for_path(filename);
	source_buf.text = file.read().get_contents();
	
	evaluate();
}

function save_file(filename)
{
	if(current_filename == "")
	{
		var file_chooser = new Gtk.FileChooserDialog();
		var file_filter = new Gtk.FileFilter();
		
		file_filter.add_mime_type("text/javascript");
		file_chooser.set_filter(file_filter);
		file_chooser.add_button("Cancel", Gtk.ResponseType.CANCEL);
		file_chooser.add_button("Save", Gtk.ResponseType.ACCEPT);
		file_chooser.set_action(Gtk.FileChooserAction.SAVE);

		if(file_chooser.run() == Gtk.ResponseType.ACCEPT)
		{
			current_filename = file_chooser.get_filename();
			window.title = "ClutterPad - " + current_filename;
		}

		file_chooser.destroy();
	}

	if(current_filename != "")
	{
		try
		{
			Gio.simple_write(current_filename, source_buf.text);
		}
		catch(e)
		{
			Seed.print(e.message);
		}
	}
}

function select_example(selector, ud)
{
	load_file("clutter-pad-examples/" + selector.get_active_text());
}

var window = new Gtk.Window({title:"ClutterPad"});
window.signal.destroy.connect(function(o) {Gtk.main_quit()});
var vbox = new Gtk.VBox();

var gtkstage = new GtkClutter.Embed();
var stage = gtkstage.get_stage();
	
stage_manager.set_default_stage(stage);
	
var right = new Gtk.VBox();

var toolbar = new Gtk.HBox();
var new_button = new Gtk.ToolButton({stock_id:"gtk-new"});
var open_button = new Gtk.ToolButton({stock_id:"gtk-open"});
var save_button = new Gtk.ToolButton({stock_id:"gtk-save"});
var example_selector = new Gtk.ComboBox.text();
var run_button = new Gtk.ToolButton({stock_id:"gtk-execute"});

new_button.signal.clicked.connect(new_file);
open_button.signal.clicked.connect(open_file);
save_button.signal.clicked.connect(save_file);
example_selector.signal.changed.connect(select_example);
run_button.signal.clicked.connect(evaluate);

toolbar.pack_start(new_button);
toolbar.pack_start(open_button);
toolbar.pack_start(save_button);
toolbar.pack_start(example_selector, true, true);
toolbar.pack_start(run_button);

populate_example_selector();

var source_lang_mgr = new GtkSource.SourceLanguageManager();
var js_lang = source_lang_mgr.get_language("js");

var source_buf = new GtkSource.SourceBuffer({language: js_lang});
var source_view = new GtkSource.SourceView.with_buffer(source_buf);

source_view.set_show_line_numbers(true);
source_view.set_show_right_margin(true);
source_view.set_highlight_current_line(true);
source_view.set_right_margin_position(80);

var scrolled_window_frame = new Gtk.Frame();
var scrolled_window = new Gtk.ScrolledWindow({vscrollbar_policy: Gtk.PolicyType.AUTOMATIC,
					      hscrollbar_policy: Gtk.PolicyType.AUTOMATIC});
scrolled_window.add(source_view);
scrolled_window_frame.add(scrolled_window);

var error_frame = new Gtk.Frame();
var error_view = new Gtk.TextView();
var error_buf = error_view.get_buffer();
error_view.set_editable(false);
error_view.set_size_request(null, 100);
error_frame.add(error_view);

right.pack_start(toolbar);
right.pack_start(scrolled_window_frame, true, true);
right.pack_start(error_frame);

var context = new sandbox.Context();
context.add_globals();

var pane = new Gtk.HPaned({position: 400});

pane.add1(gtkstage);
pane.add2(right);

vbox.pack_start(pane, true, true);

window.resize(800, 600);

window.add(vbox);

new_file();

window.show_all();

Gtk.main();
