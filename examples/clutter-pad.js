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

Gtk.init(null, null);
GtkClutter.init(null, null);

var window = new Gtk.Window();
window.signal.destroy.connect(function(o) {Gtk.main_quit()});
var gtkstage = new GtkClutter.Embed();
var stage = gtkstage.get_stage();
var vbox = new Gtk.VBox();

var right = new Gtk.VBox();
var source_lang_mgr = new GtkSource.SourceLanguageManager();
var js_lang = source_lang_mgr.get_language("js");

var source_buf = new GtkSource.SourceBuffer({language: js_lang});
var source_view = new GtkSource.SourceView.with_buffer(source_buf);
	
source_buf.text = 'Clutter = imports.gi.Clutter;\nstage = Clutter.Stage.get_default();\nvar pink = new Clutter.Color();\npink.from_string("pink");\nstage.color = pink;';
source_view.set_show_line_numbers(true);
source_view.set_show_right_margin(true);
source_view.set_highlight_current_line(true);
source_view.set_right_margin_position(80);

var run = new Gtk.Button({label: "run"});
var scrolled_window = new Gtk.ScrolledWindow({vscrollbar_policy: Gtk.PolicyType.AUTOMATIC,
					      hscrollbar_policy: Gtk.PolicyType.AUTOMATIC});
scrolled_window.add(source_view);

var error_view = new Gtk.TextView();
var error_buf = error_view.get_buffer();
error_view.set_editable(false);
error_view.set_size_request(null, 100);

right.pack_start(scrolled_window, true, true);
right.pack_start(run);
right.pack_start(error_view);

var context = new sandbox.Context();
context.add_globals();

var evaluate = function(button) {    
    try {
	error_buf.text = '';
	var a = new Gtk.TextIter(); var b = new Gtk.TextIter();
	source_buf.get_selection_bounds(a, b);
	var slice = source_buf.get_slice(a, b);
	if (slice == '') {
	    context.destroy();
	    context = new sandbox.Context();
	    context.add_globals();
	    context.eval(source_buf.text)
	} else {
	    context.eval(slice)
	}
    } catch (e) {
	error_buf.text = e.message;
    }
};

run.signal.clicked.connect(evaluate);

var pane = new Gtk.HPaned();

pane.add1(gtkstage);
pane.add2(right);

pane.set_position(400);

gtkstage.set_size_request(800,600);
vbox.pack_start(pane, true, true);

window.add(vbox);

window.show_all();
stage.show_all();

stage_manager = Clutter.StageManager.get_default();
stage_manager.set_default_stage(stage);

evaluate();
Gtk.main();
