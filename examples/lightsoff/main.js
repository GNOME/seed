#!/usr/bin/env seed

// images are 50x50

var tiles = 5;
var margin = 5;
var in_setup = false;

Seed.import_namespace("Clutter");

Clutter.init(null, null);

Seed.include("score.js");
Seed.include("light.js");
Seed.include("board.js");

function alpha_func(alpha)
{
	timeline = alpha.get_timeline();
	frame = timeline.get_current_frame();
	n_frames = timeline.num_frames;
	fps = timeline.fps;
	duration = n_frames/fps;
	time = frame/fps;

	if ((time/=duration) < (1/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625*time*time);
	else if (time < (2/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625 * (time-=(1.5/2.75))*time+.75);
	else if (time < (2.5/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625 *(time-=(2.25/2.75))*time+.9375);
	else
		return Clutter.ALPHA_MAX_ALPHA*(7.5625 * (time-=(2.625/2.75))*time+.984375);
}

function win_animation()
{
	var direction = Math.floor(2 * Math.random());
	var sign = Math.floor(2 * Math.random()) ? 1 : -1;

	var new_board = new Board();
	new_board.set_position(sign * direction * stage.width, 
						   sign * (!direction) * stage.height);
	new_board.show();
	stage.add_actor(new_board);
	new_board.lower_bottom();
	
	var fadeline = new Clutter.Timeline({num_frames:80});
	var effect = Clutter.EffectTemplate._new(fadeline, alpha_func);
	
	Clutter.effect_move(effect, new_board, 0, 0);
	var remove_line = Clutter.effect_move(effect, board, 
									-(sign)*(direction * stage.width),
									-(sign)*((!direction) * stage.height));
		
	fadeline.start();
	
	remove_line.signal.completed.connect(board.destroy_board, board);
	
	board = new_board;
}

var on_svg = Clutter.Texture.new_from_file("./tim-on.svg");
var off_svg = Clutter.Texture.new_from_file("./tim-off.svg");

on_svg.filter_quality = Clutter.TextureQuality.high;
off_svg.filter_quality = Clutter.TextureQuality.high;

var tile_size = on_svg.width + margin;

var black = Clutter.Color._new();
Clutter.color_parse("Black", black);

var stage = new Clutter.Stage({color: black});
stage.signal.hide.connect(Clutter.main_quit);
stage.set_size(tile_size * tiles + margin,
			   tile_size * tiles + margin + 105);

board = new Board();
score = new Score();
rect = new Clutter.Rectangle({color:black});

rect.set_position(0, tile_size*tiles+margin);
rect.set_size(stage.width, stage.height);

score.set_position(12, tile_size * tiles + 2*margin);
score.set_scale(0.85,0.85);

stage.add_actor(board);
stage.add_actor(rect);
stage.add_actor(score);
stage.show_all();

Clutter.main();

