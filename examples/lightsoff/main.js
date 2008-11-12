#!/usr/bin/env seed

var tiles = 5;
var margin = 5;
var in_setup = false;

var tile_size = 75;
var board_size = (tile_size + margin) * tiles + margin;


Seed.import_namespace("Clutter");
Seed.import_namespace("GtkClutter");
Seed.import_namespace("GdkPixbuf");

Clutter.init(null, null);

Seed.include("score.js");
Seed.include("light.js");
Seed.include("board.js");

var black = Clutter.Color._new();
Clutter.color_parse("Black", black);

var stage = new Clutter.Stage({color: black});
stage.signal.hide.connect(Clutter.main_quit);

board = new Board();
score = new Score();
rect = new Clutter.Rectangle({color:black});

score.set_position((board_size / 2) - (score.width / 2), board_size + margin);

rect.set_position(0, board_size);
rect.set_size(stage.width, stage.height);

stage.set_size(board_size, board_size + score.height + margin * 3);

stage.add_actor(board);
stage.add_actor(rect);
stage.add_actor(score);
stage.show_all();

Clutter.main();

