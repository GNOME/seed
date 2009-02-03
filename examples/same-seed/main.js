#!/usr/bin/env seed

// Configuration

var tiles_w = 15;
var tiles_h = 10;
var tile_size = 50;
var offset = tile_size/2;

var max_colors = 4;
var fly_score = true;

Seed.import_namespace("Gtk");
Seed.import_namespace("Clutter", "0.9");
Seed.import_namespace("GtkClutter", "0.9");
Seed.import_namespace("GdkPixbuf");
Seed.import_namespace("GConf");
Seed.import_namespace("GLib");
Seed.import_namespace("Pango");

GtkClutter.init(null, null);
GConf.init(null, null);

Seed.include("light.js");
Seed.include("board.js");
Seed.include("score.js");

var score = 0;
var timelines = [];

var window = new Gtk.Window();
var clutter = new GtkClutter.Embed();
var stage = clutter.get_stage();
var vbox = new Gtk.VBox();

window.signal.hide.connect(function () { Gtk.main_quit(); });

stage.title = "Same Seed";
stage.color = {alpha: 0};
stage.set_size((tiles_w * tile_size),(tiles_h * tile_size));

board = new Board();

stage.add_actor(board);
stage.show_all();

clutter.set_size_request(stage.width, stage.height);
vbox.pack_start(clutter, true, true);

window.add(vbox);
window.show_all();

Gtk.main();

