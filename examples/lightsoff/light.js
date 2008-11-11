LightType = {
    parent: Clutter.Group.type,
    name: "Light",
    class_init: function(klass, prototype)
    {
		prototype.flip = function ()
		{
			this.state = !this.state;
			
			if(in_setup)
			{
				this.on.opacity = this.state * 255;
				this.off.opacity = !this.state * 255;
				
				return true;
			}
			
			var fadeline = new Clutter.Timeline({fps:60, num_frames:20});
			var effect = Clutter.EffectTemplate._new(fadeline,
													 Clutter.sine_inc_func);
			
			Clutter.effect_fade(effect, this.on, this.state * 255);
			Clutter.effect_fade(effect, this.off, !this.state * 255);

			fadeline.start();

			return true;
		}
    },
    instance_init: function(klass)
    {
    	this.state = true;
    	
		this.off = new Clutter.CloneTexture({parent_texture: off_svg, 
											 reactive: true});
		this.on = new Clutter.CloneTexture({parent_texture: on_svg,
											reactive: true});
											
		this.off.opacity = 0.0;
		
		this.on.signal.button_press_event.connect(flip_region, null, this);
		this.off.signal.button_press_event.connect(flip_region, null, this);
		
		this.add_actor(this.off);
		this.add_actor(this.on);
    }};

Light = new GType(LightType);
