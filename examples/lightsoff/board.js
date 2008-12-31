var last_direction = 0;
var last_sign = 0;

function alpha_func(alpha)
{
	var timeline = alpha.get_timeline();
	var frame = timeline.get_current_frame();
	var n_frames = timeline.num_frames;
	var fps = timeline.fps;
	var duration = n_frames/fps;
	var time = frame/fps;

	if ((time/=duration) < (1/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625*time*time);
	else if (time < (2/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625*(time-=(1.5/2.75))*time+.75);
	else if (time < (2.5/2.75))
		return Clutter.ALPHA_MAX_ALPHA*(7.5625*(time-=(2.25/2.75))*time+.9375);
	else
		return Clutter.ALPHA_MAX_ALPHA*(7.5625*(time-=(2.625/2.75))*time+.984375);
}

function delete_board(timeline, board)
{
	board.destroy();
	
	animating_board = false;
	
	return true;
}

function win_animation()
{
	var direction, sign;
	
	do
	{
		direction = Math.floor(2 * Math.random());
		sign = Math.floor(2 * Math.random()) ? 1 : -1;
	}
	while(last_direction == direction && last_sign != sign);
	
	last_direction = direction;
	last_sign = sign;

	var new_board = new Board();
	new_board.set_position(sign * direction * board_size, 
						   sign * (!direction) * board_size);
	new_board.show();
	stage.add_actor(new_board);
	new_board.lower_bottom();
	
	var fadeline = new Clutter.Timeline({num_frames:80});
	var effect = Clutter.EffectTemplate._new(fadeline, alpha_func);
	
	Clutter.effect_move(effect, new_board, 0, 0);
	var remove_timeline = Clutter.effect_move(effect, board, 
						-(sign)*(direction * board_size),
						-(sign)*((!direction) * board_size));
	
	remove_timeline.signal.completed.connect(delete_board, board);
	animating_board = true;
	fadeline.start();
	
	board = new_board;
}

function swap_animation(direction)
{
	var new_board = new Board();
	new_board.show();
	stage.add_actor(new_board);
	new_board.lower_bottom();
	
	new_board.set_opacity(0);
	new_board.set_depth(direction * -250);
	
	var fadeline = new Clutter.Timeline({num_frames:40});
	var effect = Clutter.EffectTemplate._new(fadeline, Clutter.sine_inc_func);
	
	Clutter.effect_depth(effect, new_board, 0);
	var remove_timeline = Clutter.effect_depth(effect, board, direction * 250);
	Clutter.effect_fade(effect, new_board, 255);
	Clutter.effect_fade(effect, board, 0);
	
	remove_timeline.signal.completed.connect(delete_board, board);	
	animating_board = true;
	fadeline.start();
	
	board = new_board;
}

function check_won (timeline, light)
{
	in_fade = false;
	
	if(animating_board)
		return true;
	
	if(light.get_parent().cleared() && !in_setup)
	{
		score.set_value(score.get_value()+1);

		try
		{
			gconf_client.set_int("/apps/lightsoff/score", score.get_value());
		}
		catch(e)
		{
			Seed.print("Couldn't save score to GConf.");
		}

		win_animation();
	}
}

function flip_region (act, evt, light)
{
	if(!in_setup && in_fade)
		return true;
	
	var x = light.light_x;
	var y = light.light_y;
	
	var fadeline = new Clutter.Timeline({num_frames: 20});
	
	light.flip(fadeline);
	
	var lights = light.get_parent().get_lights();

	if(x + 1 < tiles)
		lights[x + 1][y].flip(fadeline);
	if(x - 1 >= 0)
		lights[x - 1][y].flip(fadeline);
	if(y + 1 < tiles)
		lights[x][y + 1].flip(fadeline);
	if(y - 1 >= 0)
		lights[x][y - 1].flip(fadeline);
	
	fadeline.start();
	in_fade = true;
	
	fadeline.signal.completed.connect(check_won, light);
	
	return true;
}

Board = new GType({
	parent: Clutter.Group.type,
	name: "Board",
	instance_init: function(klass)
	{
		// Global
		animating_board = false;
		in_fade = false;
		
		// Private
		var lights = new Array();
		
		// Public
		this.get_lights = function ()
		{
			return lights;
		}
		
		this.cleared = function ()
		{
			for(x in lights)
				for(y in lights[x])
					if(lights[x][y].get_state())
						return false;
			return true;
		}
		
		this.randomize = function ()
		{
			in_setup = true;
			
			GLib.random_set_seed(score.get_value());
			
			do
			{
				var count = Math.floor(Math.log(score.get_value() *
												score.get_value()) + 1);
				var sym = Math.floor(3 * GLib.random_double());

				for (q = 0; q < count; ++q)
				{
					i = Math.round((tiles - 1) * GLib.random_double());
					j = Math.round((tiles - 1) * GLib.random_double());

					flip_region(null, null, lights[i][j]);
					
					var x_sym_offset = Math.abs(i-(tiles-1));
					var y_sym_offset = Math.abs(j-(tiles-1));

					if(sym == 0)
						flip_region(null, null, lights[x_sym_offset][j]);
					else if(sym == 1)
						flip_region(null, null,
									lights[x_sym_offset][y_sym_offset]);
					else
						flip_region(null, null, lights[i][y_sym_offset]);
				}
			}
			while(this.cleared());

			in_setup = false;
		}

		// Implementation
		for(var x = 0; x < tiles; x++)
		{
			lights[x] = new Array();
			for(var y = 0; y < tiles; y++)
			{
				var offset = margin + tile_size/2;
				lights[x][y] = new Light();
				lights[x][y].light_x = x;
				lights[x][y].light_y = y;
				lights[x][y].set_position(x * (tile_size+margin) + offset,
										  y * (tile_size+margin) + offset);
				this.add_actor(lights[x][y]);
			}
		}
		
		this.randomize();
	}
});
	
