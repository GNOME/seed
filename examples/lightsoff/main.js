#!/usr/bin/env seed

var tiles = 5;
var margin = 5;
var in_setup = false;

var tile_size = 75;
var board_size = (tile_size + margin) * tiles + margin;

Seed.import_namespace("Gtk");
Seed.import_namespace("Clutter");
Seed.import_namespace("GtkClutter");
Seed.import_namespace("GdkPixbuf");

GtkClutter.init(null, null);

Seed.include("score.js");
Seed.include("light.js");
Seed.include("board.js");
Seed.include("menu.js");

var black = Clutter.Color._new();
Clutter.color_parse("Black", black);

var window = new Gtk.Window({title:"Lights Off"});
window.signal.hide.connect(Gtk.main_quit);

var clutter_embed = new GtkClutter.Embed();
var stage = clutter_embed.get_stage();
stage.color = black;

menu = create_menu();
board = new Board();
score = new Score();
rect = new Clutter.Rectangle({color:black});

score.set_position((board_size / 2) - (score.width / 2), board_size + margin);

stage.set_size(board_size, board_size + score.height + margin * 3);
window.resize(stage.width, stage.height);

rect.set_position(0, board_size);
rect.set_size(stage.width, stage.height);

stage.add_actor(board);
stage.add_actor(rect);
stage.add_actor(score);
stage.show_all();

window.add(clutter_embed);
window.show_all();

Gtk.main();

