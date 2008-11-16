ScoreType = {
    parent: Clutter.Group.type,
    name: "Score",
    class_init: function(klass, prototype)
    {
    	prototype.increment_value = function (val)
    	{
    		var current_val = this.value;
    		var max_val = this.value + val;
    		for(var i = this.value; i <= max_val; i += 1)
    		{
				Seed.setTimeout("score.animate_value("+i+");",5*(i - current_val) + 100);
			}
    	}
    	
    	prototype.animate_value = function (val)
		{
			this.value = val;
			
			var old_set = this.current_set;
			
			this.current_set = new Clutter.Group();

			var val_s = Seed.sprintf("% 5d",val);
		
			for(var i = 0; i < val_s.length; i++)
			{
				var texture = this.num_textures[val_s[i]];
		
				if(val_s[i] == ' ')
					texture = null;
			
				var num = new Clutter.CloneTexture({parent_texture:texture});
				num.set_position(this.num_offset * i - 3,5);
				num.set_size(this.num_size,this.num_size);
				this.current_set.add_actor(num);
			}
			
			this.add_actor(this.current_set);
			
			if(old_set)
				old_set.destroy();
			
			this.bkg_top.raise_top();
		}
    },
    instance_init: function(klass)
    {
    	this.current_set = null;
    	
    	this.bkg_top = Clutter.Texture.new_from_file("./front.svg");
		var bkg = Clutter.Texture.new_from_file("./back.svg");
		var off_svg = Clutter.Texture.new_from_file("./off.svg");
	Seed.print(bkg.__debug_ref_count());
	Seed.print(this.bkg_top.__debug_ref_count());
	Seed.print(off_svg.__debug_ref_count());

		this.bkg_top.filter_quality = Clutter.TextureQuality.high;
		bkg.filter_quality = Clutter.TextureQuality.high;
		off_svg.filter_quality = Clutter.TextureQuality.high;
		
		this.num_margin = 10;
		this.num_size = bkg.height * 0.9;
		this.num_offset = (bkg.width - 2*this.num_margin) / 5;
		
		this.add_actor(bkg);
		
		for(var i = 0; i < 5; i++)
		{
			var off_i = new Clutter.CloneTexture({parent_texture:off_svg});
			off_i.set_position(this.num_offset * i - 3,5);
			off_i.set_size(this.num_size,this.num_size);
			this.add_actor(off_i);
		}
		
		this.num_textures = new Array();
		
		for(var i = 0; i <= 9; i++)
		{
			this.num_textures[i] = Clutter.Texture.new_from_file("./"+i+".svg");
			this.num_textures[i].filter_quality = Clutter.TextureQuality.high;
		}
		
		this.bkg_top.set_position(1, 1);
		this.add_actor(this.bkg_top);

		this.animate_value(gconf_client.get_int("/apps/lightsoff/score"));
	Seed.print(bkg.__debug_ref_count());
	Seed.print(this.bkg_top.__debug_ref_count());
	Seed.print(off_svg.__debug_ref_count());

		

    }};

Score = new GType(ScoreType);

