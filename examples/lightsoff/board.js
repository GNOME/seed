var last_direction = 0;
var last_sign = 0;

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
	Clutter.effect_move(effect, board, 
						-(sign)*(direction * board_size),
						-(sign)*((!direction) * board_size));
		
	fadeline.start();
	
	board = new_board;
}

function check_won (timeline, light)
{
	in_fade = false;
	
	if(light.get_parent().cleared() && !in_setup)
	{
		score.increment_value(100);
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
	
	if(x + 1 < tiles)
		light.get_parent().lights[x + 1][y].flip(fadeline);
	if(x - 1 >= 0)
		light.get_parent().lights[x - 1][y].flip(fadeline);
	if(y + 1 < tiles)
		light.get_parent().lights[x][y + 1].flip(fadeline);
	if(y - 1 >= 0)
		light.get_parent().lights[x][y - 1].flip(fadeline);
	
	fadeline.start();
	in_fade = true;
	
	fadeline.signal.completed.connect(check_won, null, light);
	
	return true;
}

BoardType = {
    parent: Clutter.Group.type,
    name: "Board",
    class_init: function(klass, prototype)
    {
		prototype.cleared = function ()
		{
			for(x in this.lights)
				for(y in this.lights[x])
					if(this.lights[x][y].state)
						return false;
			return true;
		}
		
		prototype.randomize = function ()
		{
			in_setup = true;

			var count = Math.round(tiles*5*Math.random());
			var sym = Math.floor(3*Math.random());

			for (q = 0; q < count + 5; ++q)
			{
				i = Math.round((tiles-1) * Math.random());
				j = Math.round((tiles-1) * Math.random());

				flip_region(null, null, this.lights[i][j]);

				if(sym == 0)
					flip_region(null, null, this.lights[Math.abs(i-(tiles-1))][j]);
				else if(sym == 1)
					flip_region(null, null,
						this.lights[Math.abs(i-(tiles-1))][Math.abs(j-(tiles-1))]);
				else
					flip_region(null, null, this.lights[i][Math.abs(j-(tiles-1))]);
			}

			if(this.cleared())
				this.randomize();

			in_setup = false;
		}
    },
    instance_init: function(klass)
    {
    	in_fade = false;
    	
		this.lights = new Array();

		for(var x = 0; x < tiles; x++)
		{
			this.lights[x] = new Array();
			for(var y = 0; y < tiles; y++)
			{
				this.lights[x][y] = new Light();
				this.lights[x][y].light_x = x;
				this.lights[x][y].light_y = y;
				this.lights[x][y].set_position(x * (tile_size+margin) + margin + tile_size/2,
											   y * (tile_size+margin) + margin + tile_size/2);
				this.add_actor(this.lights[x][y]);
			}
		}
		
		this.randomize();
    }};

Board = new GType(BoardType);
