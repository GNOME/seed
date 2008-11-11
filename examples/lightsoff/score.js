#!/usr/bin/env seed

Seed.import_namespace("Clutter");

Clutter.init(null, null);

ScoreType = {
    parent: Clutter.Group.type,
    name: "Score",
    class_init: function(klass, prototype)
    {
		prototype.set_value = function (val)
		{
			var val_s = Seed.sprintf("% 5d",val);
		
			for(var i = 0; i < val_s.length; i++)
			{
				var texture = this.num_textures[val_s[i]];
		
				if(val_s[i] == ' ')
					texture = null;
			
				var num = new Clutter.CloneTexture({parent_texture:texture});
				num.set_position(56*i-10,3);
				num.set_size(97,97);
				this.add_actor(num);
				num.show();
			}
			
			this.bkg_top.raise_top();
		}
    },
    instance_init: function(klass)
    {
    	this.bkg_top = Clutter.Texture.new_from_file("./bkg_top.png");
		var bkg = Clutter.Texture.new_from_file("./bkg.png");
		var off_svg = Clutter.Texture.new_from_file("./off.svg");
		
		this.bkg_top.filter_quality = Clutter.TextureQuality.high;
		bkg.filter_quality = Clutter.TextureQuality.high;
		off_svg.filter_quality = Clutter.TextureQuality.high;
		
		this.add_actor(bkg);
		
		for(var i = 0; i < 5; i++)
		{
			var off_i = new Clutter.CloneTexture({parent_texture:off_svg});
			off_i.set_position(56*i-10,3);
			off_i.set_size(97,97);
			this.add_actor(off_i);
		}
		
		this.num_textures = new Array();
		
		for(var i = 0; i <= 9; i++)
		{
			this.num_textures[i] = Clutter.Texture.new_from_file("./"+i+".svg");
			this.num_textures[i].filter_quality = Clutter.TextureQuality.high;
		}
		
		this.bkg_top.opacity = 150;
		this.bkg_top.set_position(1,1);
		this.add_actor(this.bkg_top);
		
		this.set_value(5123);
    }};

Score = new GType(ScoreType);

var black = Clutter.Color._new();
Clutter.color_parse("Black", black);

var stage = new Clutter.Stage({color: black});stage.signal.hide.connect(Clutter.main_quit);
stage.set_size(400,400);

board = new Score();
board.set_position(20,20);
stage.add_actor(board);
stage.show_all();

Clutter.main();

