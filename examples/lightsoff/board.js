function check_won (timeline, light)
{
	if(light.get_parent().cleared() && !in_setup)
	{
		score.increment_value(100);
		win_animation();
	}
}

function flip_region (act, evt, light)
{
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
		this.lights = new Array();

		for(var x = 0; x < tiles; x++)
		{
			this.lights[x] = new Array();
			for(var y = 0; y < tiles; y++)
			{
				this.lights[x][y] = new Light();
				this.lights[x][y].light_x = x;
				this.lights[x][y].light_y = y;
				this.lights[x][y].set_position(x * tile_size + margin + tile_size/2,
											   y * tile_size + margin + tile_size/2);
				this.add_actor(this.lights[x][y]);
			}
		}
		
		this.randomize();
    }};

Board = new GType(BoardType);
