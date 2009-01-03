Arrow = new GType({
	parent: Clutter.Group.type,
	name: "Arrow",
	instance_init: function(klass)
	{
		// Private
		var direction = 0;
		
		var toggle_arrow = function (actor, event)
		{
			if(animating_board)
				return true;

			if(score.set_value(score.get_value() + direction))
				swap_animation(direction);
	
			return true;
		}
		
		// Public
		this.set_arrow_direction = function (dir)
		{
			direction = dir ? 1 : -1;
			
			var bkg = Clutter.Texture.new_from_file("./arrow-" + 
													(dir ? "r" : "l") + ".svg");
			
			bkg.filter_quality = Clutter.TextureQuality.High;
			this.add_actor(bkg);
		}

		// Implementation
		this.reactive = true;
		this.signal.button_press_event.connect(toggle_arrow);
	}
});

