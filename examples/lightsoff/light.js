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
							
		this.on.opacity = 0.0;
		
		this.set_anchor_point(tile_size / 2, tile_size / 2);

		this.on.signal.button_press_event.connect(flip_region, null, this);
		this.off.signal.button_press_event.connect(flip_region, null, this);
		
		this.add_actor(this.off);
		this.add_actor(this.on);
		
    }};

Light = new GType(LightType);
