#!/usr/bin/env seed

Gtk = imports.gi.Gtk;
Clutter = imports.gi.Clutter;
GtkSource = imports.gi.GtkSource;
Gio = imports.gi.Gio;

var source_lang_mgr = new GtkSource.SourceLanguageManager();
var js_lang = source_lang_mgr.get_language("c");

ShaderEditor = new GType({
	parent: Gtk.HBox.type,
	name: "ShaderEditor",
	init: function()
	{
		// Private
		
		var source_buffer = new GtkSource.SourceBuffer({language: js_lang});
		var source_view = new GtkSource.SourceView.with_buffer(source_buffer);
		var compile_button = new Gtk.Button({label: "Compile"});
		var scrolled_window = new Gtk.ScrolledWindow({vscrollbar_policy: Gtk.PolicyType.AUTOMATIC,
													  hscrollbar_policy: Gtk.PolicyType.AUTOMATIC});
		
		// Public
		
		this.connect_shader_view = function (shader_view)
		{
			compile_button.signal.clicked.connect(shader_view.run_shader, source_buffer);
		};
		
		// Implementation
		
		source_buffer.text = Gio.simple_read("default.glsl");
		
		source_view.set_show_line_numbers(true);
		source_view.set_show_right_margin(true);
		source_view.set_highlight_current_line(true);
		source_view.set_right_margin_position(80);
		
		scrolled_window.add(source_view);
		
		this.pack_start(scrolled_window, true, true);
		this.pack_start(compile_button);
	}
});

