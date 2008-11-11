#!/usr/bin/env seed

// images are 50x50

var tiles = 5;
var in_setup = false;

Seed.import_namespace("Clutter");

Clutter.init(null, null);

Seed.include("light.js");
Seed.include("board.js");

flip_region = function (act, evt, light)
{
	var x = light.light_x;
	var y = light.light_y;
	
	light.flip();
	
	if(x + 1 < tiles)
		light.get_parent().lights[x + 1][y].flip();
	if(x - 1 >= 0)
		light.get_parent().lights[x - 1][y].flip();
	if(y + 1 < tiles)
		light.get_parent().lights[x][y + 1].flip();
	if(y - 1 >= 0)
		light.get_parent().lights[x][y - 1].flip();
	
	if(board.cleared() && !in_setup)
		Seed.print("Glorious victory!");
	
	return true;
}

function random_clicks()
{
	in_setup = true;
	
	//var count = Math.round(tiles*5* Math.random());
	var count = -4;
	
	var sym = Math.floor(3*Math.random());

	for (q = 0; q < count + 5; ++q)
	{
		i = Math.round((tiles-1) * Math.random());
		j = Math.round((tiles-1) * Math.random());
	    
		flip_region(null, null, board.lights[i][j]);
	    
		if(sym == 0)
			flip_region(null, null, board.lights[Math.abs(i-(tiles-1))][j]);
		else if(sym == 1)
			flip_region(null, null,
						board.lights[Math.abs(i-(tiles-1))][Math.abs(j-(tiles-1))]);
		else
			flip_region(null, null, board.lights[i][Math.abs(j-(tiles-1))]);
	}
	
	in_setup = false;
	
	// do it again if you won already
	if(board.cleared())
		random_clicks();
}

function win_animation()
{
	
}

var on_svg = Clutter.Texture.new_from_file("./tim-on.svg");
var off_svg = Clutter.Texture.new_from_file("./tim-off.svg");

var black = Clutter.Color._new();
Clutter.color_parse("Black", black);

var stage = new Clutter.Stage({color: black});
stage.signal.hide.connect(Clutter.main_quit);
stage.set_size(55 * tiles + 5, 55 * tiles + 5);

board = new Board();

stage.add_actor(board);
stage.show_all();

random_clicks();

Clutter.main();

