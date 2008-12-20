#!/usr/bin/env seed

// Configuration
var tiles = 5;
var tile_size = 75;

Seed.import_namespace("Gtk");
Seed.import_namespace("Clutter");
Seed.import_namespace("GtkClutter");
Seed.import_namespace("GdkPixbuf");
Seed.import_namespace("GConf");
Seed.import_namespace("GLib");

Clutter.init(null, null);
GConf.init(null, null);

var margin = 5;
var in_setup = false;
var board_size = (tile_size + margin) * tiles + margin;

Seed.include("score.js");
Seed.include("light.js");
Seed.include("board.js");
Seed.include("arrow.js");

var gconf_client = GConf.Client.get_default();

var black = Clutter.Color._new();
Clutter.color_parse("Black", black);

var stage = new Clutter.Stage();
stage.signal.hide.connect(function(){Clutter.main_quit()});
stage.color = black;

score = new Score();
board = new Board();
rect = new Clutter.Rectangle({color:black});
forward = new Arrow();
back = new Arrow();

score.set_position((board_size / 2) - (score.width / 2), board_size + margin);
stage.set_size(board_size, board_size + score.height + margin * 3);

rect.set_position(0, board_size);
rect.set_size(stage.width, stage.height);

back.set_position(score.x - back.width - 2*margin, score.y + (.5 * score.height) - (.5 * back.height));

forward.set_arrow_flipped();
forward.set_position(score.x + score.width + 2*margin, score.y + (.5 * score.height) - (.5 * forward.height));

stage.add_actor(board);
stage.add_actor(rect); // TODO: gradient at top of rect
stage.add_actor(score);
stage.add_actor(forward);
stage.add_actor(back);
stage.show_all();

Clutter.main();

