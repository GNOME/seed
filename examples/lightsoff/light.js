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
				this.off.opacity = !this.state * 255;

				return true;
			}
			
			var effect = Clutter.EffectTemplate._new(fadeline,
													 Clutter.sine_inc_func);
			
			Clutter.effect_fade(effect, this.on, this.state * 255);
			Clutter.effect_fade(effect, this.off, !this.state * 255);

			Clutter.effect_rotate(effect, this, Clutter.RotateAxis.x_axis, 360, 25, 25, 25);
			//Clutter.effect_rotate(effect, this, Clutter.RotateAxis.y_axis, 180, 25, 0, 0);

			return true;
		}
    },
    instance_init: function(klass)
    {
    	this.state = false;
    	
    	this.on = new Clutter.CloneTexture({parent_texture: on_svg,
											reactive: true});
		this.off = new Clutter.CloneTexture({parent_texture: off_svg, 
											 reactive: true});
							
		this.on.opacity = 0.0;

		this.on.signal.button_press_event.connect(flip_region, null, this);
		this.off.signal.button_press_event.connect(flip_region, null, this);
		
		this.add_actor(this.on);
		this.add_actor(this.off);
		
    }};

Light = new GType(LightType);
