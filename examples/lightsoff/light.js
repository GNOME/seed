var tile_svg_size = 200;

var on_pixbuf = GdkPixbuf.Pixbuf.new_from_file_at_size("./light-on.svg", tile_svg_size, tile_svg_size);
var off_pixbuf = GdkPixbuf.Pixbuf.new_from_file_at_size("./light-off.svg", tile_svg_size, tile_svg_size);

var on_svg = GtkClutter.texture_new_from_pixbuf(on_pixbuf);
var off_svg = GtkClutter.texture_new_from_pixbuf(off_pixbuf);

on_svg.filter_quality = Clutter.TextureQuality.high;
off_svg.filter_quality = Clutter.TextureQuality.high;

LightType = {
    parent: Clutter.Group.type,
    name: "Light",
    class_init: function(klass, prototype)
    {
		prototype.flip = function (fadeline)
		{
			this.state = !this.state;
			
			if(in_setup)
			{
				this.on.opacity = this.state * 255;
				
				this.scale_x = this.state?1:.9;
				this.scale_y = this.state?1:.9;
				
				return true;
			}
			
			var effect = Clutter.EffectTemplate._new(fadeline,
													 Clutter.sine_inc_func);
			
			Clutter.effect_fade(effect, this.on, this.state * 255);
			Clutter.effect_scale(effect, this, this.state?1:.9, this.state?1:.9);
			
			return true;
		}
    },
    instance_init: function(klass)
    {
    	this.state = false;
    	
    	this.scale_x = .9;
		this.scale_y = .9;
    	
    	this.on = new Clutter.CloneTexture({parent_texture: on_svg,
											reactive: true});
		this.off = new Clutter.CloneTexture({parent_texture: off_svg, 
											 reactive: true});
		
		this.on.set_size(tile_size, tile_size);
		this.off.set_size(tile_size, tile_size);
		
		this.on.opacity = 0.0;
		
		this.set_anchor_point(tile_size / 2, tile_size / 2);

		this.on.signal.button_press_event.connect(flip_region, this);
		this.off.signal.button_press_event.connect(flip_region, this);
		
		this.add_actor(this.off);
		this.add_actor(this.on);
		
    }};

Light = new GType(LightType);
