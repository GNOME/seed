var bkg_top = Clutter.Texture.new_from_file("./lcd-front.svg");
var bkg = Clutter.Texture.new_from_file("./lcd-back.svg");

bkg_top.filter_quality = bkg.filter_quality = Clutter.TextureQuality.High;

var num_margin = 7;

Score = new GType({
	parent: Clutter.Group.type,
	name: "Score",
	instance_init: function(klass)
	{
		// Private
		var value = 0;
		var current_set = null;
		
		var num_width = bkg.height * 0.9 * 0.625;
		var num_height = bkg.height * 0.9;
		var num_offset = num_width;
		
		var num_textures = new Array();
		
		var off_svg = Clutter.Texture.new_from_file("./lcd-off.svg");
		off_svg.filter_quality = Clutter.TextureQuality.High;
		
		// Public
		this.get_value = function ()
		{
			return value;
		};
		
		this.set_value = function (val)
		{
			if(val < 1)
				return false;
			
			value = val;
			
			try
			{
				gconf_client.set_int("/apps/lightsoff/score", value);
			}
			catch(e)
			{
				Seed.print("Couldn't save score to GConf.");
			}
			
			var old_set = current_set;
			
			current_set = new Clutter.Group();

			var val_s = Seed.sprintf("% 5d", val);
		
			for(var i = 0; i < val_s.length; i++)
			{
				var texture = num_textures[val_s[i]];
		
				if(val_s[i] == ' ')
					texture = null;
			
				var num = new Clutter.CloneTexture({parent_texture:texture});
				num.show();
				num.set_position(num_margin + num_offset * i, 5);
				num.set_size(num_width, num_height);
				current_set.add_actor(num);
			}
			
			this.add_actor(current_set);
			current_set.show();
			
			if(old_set)
				old_set.destroy();
			
			bkg_top.raise_top();
			
			return true;
		};
		
		// Implementation
		this.add_actor(bkg);
		
		for(var i = 0; i < 5; i++)
		{
			var off_i = new Clutter.CloneTexture({parent_texture:off_svg});
			off_i.set_position(num_margin + num_offset * i, 5);
			off_i.set_size(num_width, num_height);
			this.add_actor(off_i);
		}
		
		for(var i = 0; i <= 9; i++)
		{
			num_textures[i] = Clutter.Texture.new_from_file("./"+i+".svg");
			num_textures[i].filter_quality = Clutter.TextureQuality.High;
		}
		
		bkg_top.set_position(1, 1);
		this.add_actor(bkg_top);
		
		try
		{
			gconf_client = GConf.Client.get_default();
			this.set_value(gconf_client.get_int("/apps/lightsoff/score"));
		}
		catch(e)
		{
			Seed.print("Couldn't load score from GConf.");
			this.set_value(1);
		}
	}
});

