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
		var cl, oldpicked;
		
		function _light_connected_lights(li)
		{
			// Anything anyone can do to make this function faster
			// will make the game snappier.
			
			if(!li || li.visited || li.get_closed())
				return [ ];
			
			var x = li.get_light_x();
			var y = li.get_light_y();
			
			li.visited = true;
			
			var con = [li];
			
			// Do all of the concatenation together for performance
			
			var a = [], b = [], c = [], d = [];
			
			if(lights[x][y+1] && (li.get_state() == lights[x][y+1].get_state()))
				a = _light_connected_lights(lights[x][y+1]);
			
			if(lights[x][y-1] && (li.get_state() == lights[x][y-1].get_state()))
				b = _light_connected_lights(lights[x][y-1]);
			
			if(lights[x+1] && (li.get_state() == lights[x+1][y].get_state()))
				c = _light_connected_lights(lights[x+1][y]);
			
			if(lights[x-1] && (li.get_state() == lights[x-1][y].get_state()))
				d = _light_connected_lights(lights[x-1][y]);
			
			return con.concat(a,b,c,d);
		};
		
		function light_connected_lights(li)
		{
			for(var i in all_lights)
			{
				all_lights[i].visited = false;
			}
			
			if(!li.get_light_x) // We're picking something other than a light!
				return [ li ];
			
			return _light_connected_lights(li);
		};
		
		function any_connected_lights(li)
		{
			if(!li)
				return false;
			
			var x = li.get_light_x();
			var y = li.get_light_y();
			
			if(li.get_closed())
				return false;
			
			if(lights[x][y+1] && (li.get_state() == lights[x][y+1].get_state()))
				return true;

			if(lights[x][y-1] && (li.get_state() == lights[x][y-1].get_state()))
				return true;
			
			if(lights[x+1] && (li.get_state() == lights[x+1][y].get_state()))
				return true;
			
			if(lights[x-1] && (li.get_state() == lights[x-1][y].get_state()))
				return true;
			
			return false;
		};
		
		function calculate_score(n_lights)
		{
			if (n_lights < 3)
				return 0;

			return (n_lights - 2) * (n_lights - 2);
		};
		
		var mouse_moved = function (actor, event)
		{
			var picked = stage.get_actor_at_pos(event.motion.x,
												event.motion.y).get_parent();
			
			if(picked == oldpicked)
				return;
			
			for(var i in all_lights)
			{
				all_lights[i].opacity = 180;
			}
			
			cl = light_connected_lights(picked);
			
			if(cl.length < 2)
				return false;
			
			for(var i in cl)
			{
				cl[i].opacity = 255;
			}
			
			return false;
		};
		
		// Public
		this.has_completed = function ()
		{
			for(var i in all_lights)
			{
				li = all_lights[i];
				
				// For some reason, any_connected_lights isn't always right...
				
				if(!li.get_closed() && any_connected_lights(li))
					return false;
			}
			
			return true;
		};
		
		this.has_won = function ()
		{
			for(var i in all_lights)
			{
				li = all_lights[i];
				
				if(!li.get_closed())
					return false;
			}
			
			return true;
		};
		
		this.get_lights = function ()
		{
			return lights;
		};
		
		this.remove_region = function (actor, event)
		{
			if(cl.length < 2)
				return false;
			
			for(var i in cl)
			{
				cl[i].close_tile();
			}
			
			var real_x = 0;
			
			for(var x in lights)
			{
				var good_lights = [];
				var bad_lights = [];
				
				for(var y in lights[x])
				{
					var li = lights[x][y];
					
					if(!li.get_closed())
						good_lights.push(li);
					else
						bad_lights.push(li);
				}
				
				lights[real_x] = good_lights.concat(bad_lights);
				
				var empty_col = true;
				
				for(var y in lights[real_x])
				{
					var li = lights[real_x][y];
					
					li.set_light_x(real_x);
					li.set_light_y(parseInt(y,10));
					
					li.anim = li.animate(Clutter.AnimationMode.EASE_OUT_BOUNCE, 500,
					{
						x: [GObject.TYPE_INT, real_x * tile_size + offset],
						y: [GObject.TYPE_INT, (tiles_h - y - 1) * tile_size + offset]
					});
					li.anim.timeline.start();
					
					if(!li.get_closed())
						empty_col = false;
				}
				
				if(!empty_col)
					real_x++;
			}
			
			for(;real_x < tiles_w; real_x++)
				lights[real_x] = null;
			
			score += calculate_score(cl.length);
			
			Seed.print(score);
			
			if(board.has_completed())
			{
				if(board.has_won())
					score += 1000;
				
				Seed.print("Done with: " + score + " points!");
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
				var li = new Light();
				
				li.set_light_x(x);
				li.set_light_y(y);
				
				li.set_position(x * tile_size + offset,
								(tiles_h - y - 1) * tile_size + offset);
				this.add_actor(li);
				li.on.signal.button_press_event.connect(this.remove_region);
				
				lights[x][y] = li;
				all_lights.push(lights[x][y]);
			}
		}
		
		this.signal.motion_event.connect(mouse_moved);
		this.reactive = true;
	}
});
	
