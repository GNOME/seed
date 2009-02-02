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
	
	return true;
}

Board = new GType({
	parent: Clutter.Group.type,
	name: "Board",
	init: function(klass)
	{
		// Global

		
		// Private
		var lights = new Array();
		var all_lights = new Array();
		
		function _connected_lights(li)
		{
			if(!li)
				return [ ];
			
			var x = li.get_light_x();
			var y = li.get_light_y();
			
			//Seed.printf("%d %d " + li.toString(), x, y);
			//Seed.print(lights[x][y+1]);
			
			if(li.visited || li.get_closed())
				return [ ];
			
			li.visited = true;
			
			var con = [li];
			
			// doesn't deal with state of tile yet
			
			if(lights[x][y+1] && (li.get_state() == lights[x][y+1].get_state()))
				con = con.concat(_connected_lights(lights[x][y+1]));
			
			if(lights[x][y-1] && (li.get_state() == lights[x][y-1].get_state()))
				con = con.concat(_connected_lights(lights[x][y-1]));
			
			if(lights[x+1] && (li.get_state() == lights[x+1][y].get_state()))
				con = con.concat(_connected_lights(lights[x+1][y]));
			
			if(lights[x-1] && (li.get_state() == lights[x-1][y].get_state()))
				con = con.concat(_connected_lights(lights[x-1][y]));
			
			return con;
		};
		
		function connected_lights(li)
		{
			for(i in all_lights)
			{
				all_lights[i].visited = false;
			}
			
			if(!li.get_light_x) // We're picking something other than a light!
				return [ li ];
			
			return _connected_lights(li);
		};
		
		var mouse_moved = function (actor, event)
		{
			var picked = stage.get_actor_at_pos(event.motion.x,
												event.motion.y).get_parent();
			
			for(i in all_lights)
			{
				all_lights[i].opacity = 180;
			}
			
			var cl = connected_lights(picked);
			
			for(i in cl)
			{
				cl[i].opacity = 255;
			}
			
			return false;
		};
		
		// Public
		this.get_lights = function ()
		{
			return lights;
		}
		
		this.remove_region = function (actor, event, light)
		{
			if(event.button.button == 2)
			{
				x = light.get_light_x();
				y = light.get_light_y();
				
				Seed.print(x + " " + y);
				
				for(i in lights[x])
					Seed.print(x + " " + i + " " + lights[x][i].get_closed() + " " + (i == y));
				
				Seed.print(lights[x].length);
				
				if(lights[x][y+1])
					Seed.print("up " + lights[x][y+1].get_closed());
				else
					Seed.print(x + " " + (y+1) + " " +lights[x][y+1]);
				
				if(lights[x][y-1])
					Seed.print("down " + lights[x][y-1].get_closed());
				else
					Seed.print(x + " " + (y-1) + " " +lights[x][y-1]);
				
				if(lights[x-1] && lights[x-1][x])
					Seed.print("left " + lights[x-1][y].get_closed());
				else
					Seed.print((x-1) + " " + y + " " +lights[x-1]);
				
				if(lights[x+1] && lights[x+1][y])
					Seed.print("right " + lights[x+1][y].get_closed());
				else
					Seed.print((x+1) + " " + y + " " +lights[x+1]);
				
				return false;
			}
		
			if(event.button.button == 3)
			{
				var str = [];
				
				for(var i = 0; i < tiles_h; i++)
					str[i] = "";
				
				for(x in lights)
				{
					for(y in lights[x])
						//if(lights[x][y].get_closed())
						//	str[y] += " ";
						//else
							str[y] += lights[x][y].get_state();
				}
				
				for(i in str)
					Seed.print(str[i]);
				
				return false;
			}
		
			var cl = connected_lights(light);
			
			for(i in cl)
			{
				cl[i].close_tile();
			}
			
			for(x in lights)
			{
				var good_lights = [];
				var bad_lights = [];
				
				for(y in lights[x])
				{
					var li = lights[x][y];
					
					if(!li.get_closed())
						good_lights.push(li);
					else
						bad_lights.push(li);
				}
				
				lights[x] = good_lights.concat(bad_lights);
				
				for(y in lights[x])
				{
					lights[x][y].set_light_y(parseInt(y,10));
					
					Seed.print(lights[x][y].get_light_y() + 2);
				
					lights[x][y].set_position(x * tile_size + offset,
											  (tiles_h - y - 1) * tile_size + offset);
				}
				
				
				
				
			}
			
			return false;
		}
		
		// Implementation
		for(var x = 0; x < tiles_w; x++)
		{
			lights[x] = new Array();
			for(var y = 0; y < tiles_h; y++)
			{
				var offset = tile_size/2;
				lights[x][y] = new Light();
				all_lights.push(lights[x][y]);
				
				lights[x][y].set_light_x(x);
				lights[x][y].set_light_y(y);
				
				lights[x][y].set_position(x * tile_size + offset,
										  (tiles_h - y - 1) * tile_size + offset);
				this.add_actor(lights[x][y]);
				lights[x][y].on.signal.button_press_event.connect(this.remove_region,
																  lights[x][y]);
			}
		}
		
		this.signal.motion_event.connect(mouse_moved);
		this.reactive = true;
	}
});
	
