#!/usr/bin/env seed

var tiles_w = 15;
var tiles_h = 10;
var tile_size = 50;

// Configuration
Seed.import_namespace("Gtk");
Seed.import_namespace("Clutter", "0.9");
Seed.import_namespace("GtkClutter", "0.9");
Seed.import_namespace("GdkPixbuf");
Seed.import_namespace("GConf");
Seed.import_namespace("GLib");

Clutter.init(null, null);
GConf.init(null, null);

Seed.include("light.js");
Seed.include("board.js");

var black = new Clutter.Color();
Clutter.color_parse("Black", black);

var stage = new Clutter.Stage();

stage.title = "Same Seed";
stage.signal.hide.connect(function () { Clutter.main_quit(); });
stage.color = black;
stage.set_size((tiles_w * tile_size),(tiles_h * tile_size));

colors = [load_svg("blue.svg"), load_svg("green.svg"),
		  load_svg("red.svg"), load_svg("yellow.svg")]

board = new Board();

stage.add_actor(board);
stage.show_all();

Clutter.main();

