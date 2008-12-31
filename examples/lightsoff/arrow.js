Arrow = new GType({
	parent: Clutter.Group.type,
	name: "Arrow",
	instance_init: function(klass)
	{
		var flipped = 0;
		
		this.set_arrow_direction = function (dir)
		{
			var bkg;
			flipped = dir;

			if(dir)
				bkg = Clutter.Texture.new_from_file("./arrow-r.svg");
			else
				bkg = Clutter.Texture.new_from_file("./arrow-l.svg");
			
			bkg.filter_quality = Clutter.TextureQuality.High;
			this.add_actor(bkg);
			bkg.show();
		}
		
		var toggle_arrow = function (actor, event)
		{
			if(animating_board)
				return true;
	
			var direction = (flipped ? 1 : -1);
	
			if(score.get_value() + direction < 1)
				return true;
	
			score.set_value(score.get_value() + direction);
			swap_animation(direction);

			try
			{
				gconf_client.set_int("/apps/lightsoff/score", score.get_value());
			}
			catch(e)
			{
				Seed.print("Couldn't save score to GConf.");
			}
	
			return true;
		}

		this.reactive = true;
		this.signal.button_press_event.connect(toggle_arrow);
	}
});

