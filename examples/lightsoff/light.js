var tile_svg_size = 100;

var on_pixbuf = new GdkPixbuf.Pixbuf.from_file_at_size("./light-on.svg",
													   tile_svg_size,
													   tile_svg_size);
var off_pixbuf = new GdkPixbuf.Pixbuf.from_file_at_size("./light-off.svg",
														tile_svg_size,
														tile_svg_size);

var on_svg = GtkClutter.texture_new_from_pixbuf(on_pixbuf);
var off_svg = GtkClutter.texture_new_from_pixbuf(off_pixbuf);

on_svg.filter_quality = off_svg.filter_quality = Clutter.TextureQuality.High;

Light = new GType({
	parent: Clutter.Group.type,
	name: "Light",
	instance_init: function(klass)
	{
		// Private
		var state = false;
		var light_x, light_y;
		
		// Public
		this.scale_x = this.scale_y = .9;
		
		this.on = new Clutter.CloneTexture({parent_texture: on_svg,
											reactive: true,
											opacity: 0});
		this.off = new Clutter.CloneTexture({parent_texture: off_svg, 
											 reactive: true});
		
		this.get_state = function ()
		{
			return state;
		}
		
		this.flip = function (fadeline)
		{
			state = !state;
			
			var new_scale = state ? 1 : .9;
			
			if(in_setup)
			{
				this.on.opacity = state * 255;
				this.scale_x = this.scale_y = new_scale;
				
				return;
			}
			
			var effect = 
  			     new Clutter.EffectTemplate.c_new(fadeline,
												  Clutter.sine_inc_func);
			
			Clutter.effect_fade(effect, this.on, state * 255);
			Clutter.effect_scale(effect, this, new_scale, new_scale);
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
		this.off.set_size(tile_size, tile_size);
		
		this.set_anchor_point(tile_size / 2, tile_size / 2);

		this.on.signal.button_press_event.connect(flip_region, this);
		this.off.signal.button_press_event.connect(flip_region, this);
		
		this.add_actor(this.off);
		this.add_actor(this.on);
	}
});

