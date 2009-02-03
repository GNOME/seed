var tile_svg_size = 50;

function load_svg(file)
{
	var pb = new GdkPixbuf.Pixbuf.from_file_at_size(file, tile_svg_size,
													tile_svg_size);
	var tx = GtkClutter.texture_new_from_pixbuf(pb);
	tx.filter_quality = Clutter.TextureQuality.HIGH;
	
	stage.add_actor(tx);
	tx.x = tx.y = -2000;
	
	return tx;
}

var colors;

Light = new GType({
	parent: Clutter.Group.type,
	name: "Light",
	init: function(klass)
	{
		// Private
		var closed = false;
		var light_x, light_y;
		var state = Math.floor(Math.random() * colors.length);
		
		// Public
		this.visited = false;
		
		this.on = new Clutter.Clone({source: colors[state],
									 reactive: true});
		
		this.get_state = function ()
		{
			return state;
		}
		
		this.get_closed = function ()
		{
			return closed;
		}
		
		this.close_tile = function ()
		{
			closed = true;
		}
		
		this.hide_light = function (timeline, light)
		{
			light.hide();
			
			delete on;
			
			return false;
		}
		
		this.set_light_x = function (new_x)
		{
			light_x = new_x;
		}
		
		this.set_light_y = function (new_y)
		{
			light_y = new_y;
		}
		
		this.get_light_x = function ()
		{
			return light_x;
		}
		
		this.get_light_y = function ()
		{
			return light_y;
		}
		
		// Implementation
		this.on.set_size(tile_size, tile_size);
		
		this.opacity = 180;
		
		this.set_anchor_point(tile_size / 2, tile_size / 2);
		
		this.add_actor(this.on);
	}
});

