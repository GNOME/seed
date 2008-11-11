#!/usr/bin/env seed

Seed.import_namespace("Clutter");

Clutter.init(null, null);

ScoreType = {
    parent: Clutter.Group.type,
    name: "Score",
    class_init: function(klass, prototype)
    {

    },
    instance_init: function(klass)
    {
		var bkg = Clutter.Texture.new_from_file("./bkg.png");
		var off_svg = Clutter.Texture.new_from_file("./off.svg");
		
		bkg.filter_quality = Clutter.TextureQuality.high;
		off_svg.filter_quality = Clutter.TextureQuality.high;
		
		this.add_actor(bkg);
		
		for(var i = 0; i < 5; i++)
		{
			var off_i = new Clutter.CloneTexture({parent_texture:off_svg});
			off_i.set_position(50*i-10,0);
			off_i.set_size(80,80);
			this.add_actor(off_i);
		}
		
		var seven = Clutter.Texture.new_from_file("./7.svg");
		seven.filter_quality = Clutter.TextureQuality.high;
		seven.set_position(-10,0);
		seven.set_size(80,80);
		this.add_actor(seven);
    }};

Score = new GType(ScoreType);

var stage = new Clutter.Stage();
stage.signal.hide.connect(Clutter.main_quit);
stage.set_size(400,400);

board = new Score();

stage.add_actor(board);
stage.show_all();

Clutter.main();

