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
Seed.import_namespace("GConf");
Seed.import_namespace("GLib");

GtkClutter.init(null, null);
GConf.init(null, null);

Seed.include("score.js");
Seed.include("light.js");
Seed.include("board.js");
Seed.include("menu.js");
Seed.include("arrow.js");

var gconf_client = GConf.client_get_default();

var black = Clutter.Color._new();
Clutter.color_parse("Black", black);

var window = new Gtk.Window({title:"Lights Off"});
window.signal.hide.connect(Gtk.main_quit);

var clutter_embed = new GtkClutter.Embed();
var stage = clutter_embed.get_stage();
stage.color = black;

menu = create_menu();
score = new Score();
board = new Board();
rect = new Clutter.Rectangle({color:black});
forward = new Arrow();
back = new Arrow();

score.set_position((board_size / 2) - (score.width / 2), board_size + margin);
stage.set_size(board_size, board_size + score.height + margin * 3);
window.resize(stage.width, stage.height);

rect.set_position(0, board_size);
rect.set_size(stage.width, stage.height);

back.set_position(score.x - back.width - 2*margin, score.y + (.5 * score.height) - (.5 * back.height));

forward.set_arrow_flipped();
forward.set_position(score.x + score.width, score.y + (.5 * score.height) - (.5 * forward.height));

stage.add_actor(board);
stage.add_actor(rect); // TODO: gradient at top of rect
stage.add_actor(score);
stage.add_actor(forward);
stage.add_actor(back);
stage.show_all();

window.add(clutter_embed);
window.show_all();

Gtk.main();

