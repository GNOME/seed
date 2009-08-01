#!/usr/bin/env seed

Clutter = imports.gi.Clutter;
Gtk = imports.gi.Gtk;
GtkSource = imports.gi.GtkSource;
GtkClutter = imports.gi.GtkClutter;
Gio = imports.gi.Gio;
ShaderEditor = imports.ShaderEditor;
ShaderView = imports.ShaderView;

Gtk.init(Seed.argv);
GtkClutter.init(Seed.argv);

var window = new Gtk.Window();
window.signal.hide.connect(Gtk.main_quit);

var gtkstage = new GtkClutter.Embed();
var stage = gtkstage.get_stage();

gtkstage.set_size_request(500,500);
stage.color = {alpha: 1};

var shader_view = new ShaderView.ShaderView();

var editor = new ShaderEditor.ShaderEditor();
editor.connect_shader_view(shader_view);

var edbin = new Gtk.Frame();
edbin.add(editor);

var pane = new Gtk.VPaned({position: 400});
pane.add1(gtkstage);
pane.add2(edbin);
window.add(pane);

stage.add_actor(shader_view);

window.show_all();

shader_view.resize(stage.width, stage.height);

Gtk.main();
