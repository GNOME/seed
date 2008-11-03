#!/usr/local/bin/seed
Seed.import_namespace("Clutter");
Seed.import_namespace("Gtk");
Seed.import_namespace("GtkSource");
Seed.import_namespace("GtkClutter");
Seed.import_namespace("Gio");
Seed.include("ShaderView.js");

Gtk.init(null, null);
GtkClutter.init(null, null);

var window = new Gtk.Window();
var gtkstage = new GtkClutter.Embed();
var stage = gtkstage.get_stage();
var texture = new Clutter.Texture({filename:"bob.jpg"});
var reflection = new Clutter.CloneTexture({parent_texture: texture});
shader = new Clutter.Shader();
var shaderfield = new ShaderView("fragment_source", texture,reflection);
var vbox = new Gtk.VBox();
var notebook = new Gtk.Notebook();
var c = Clutter.Color._new();
Clutter.color_parse("Black",c);

notebook.append_page(shaderfield.hbox, new Gtk.Label({label:"Fragment"}));
reflection.width = reflection.height = texture.width = texture.height = 300;


gtkstage.set_size_request(500,500);
vbox.pack_start(gtkstage, false, true);
vbox.pack_start(notebook, true, true);

window.add(vbox);
stage.add_actor(texture);
stage.add_actor(reflection);

reflection.anchor_x = texture.anchor_x = texture.width/2;
reflection.anchor_y = texture.anchor_y = texture.height/2;
texture.x = stage.width/2-texture.width/4;
texture.y = stage.height/2;

reflection.x = stage.width/2-texture.width/4;
reflection.y = stage.height/2+texture.height;
reflection.rotation_angle_z = 180;
reflection.opacity = 80;

stage.color = c;

window.show_all();
stage.show_all();

Gtk.main();
