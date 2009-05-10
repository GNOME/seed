#!/usr/bin/env seed

// Configuration

var tiles_w = 15;
var tiles_h = 10;
var tile_size = 50;
var offset = tile_size/2;

var max_colors = 3;
var fly_score = true;

imports.gi.versions.Clutter = "0.9";

Gtk = imports.gi.Gtk;
Clutter = imports.gi.Clutter;
GdkPixbuf = imports.gi.GdkPixbuf;
GConf = imports.gi.GConf;
GLib = imports.gi.GLib;
Pango = imports.gi.Pango;
GObject = imports.gi.GObject;

Gtk.init(null, null);
Clutter.init(null, null);
GConf.init(null, null);

Seed.include("light.js");
Seed.include("board.js");
Seed.include("score.js");

var score = 0;
var timelines = [];

var stage = new Clutter.Stage();

stage.signal.hide.connect(Clutter.main_quit);

stage.color = {alpha: 0};
stage.set_size((tiles_w * tile_size),(tiles_h * tile_size));

var board = new Board();
stage.add_actor(board);
stage.show_all();

board.new_game();

Clutter.main();

