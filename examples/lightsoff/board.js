BoardType = {
    parent: Clutter.Group.type,
    name: "Board",
    class_init: function(klass, prototype)
    {
		prototype.cleared = function ()
		{
			for(x in this.lights)
				for(y in this.lights[x])
					if(!this.lights[x][y].state)
						return false;
			return true;
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
				this.lights[x][y].set_position(x * 55 + 5, y * 55 + 5);
				this.add_actor(this.lights[x][y]);
			}
		}
    }};

Board = new GType(BoardType);
